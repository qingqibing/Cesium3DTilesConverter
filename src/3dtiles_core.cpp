#include <3dtiles_core.h>
#include <QMetaEnum>


namespace gzpi {
   



       
        TileMatrix::TileMatrix() {

        }

        QJsonValue TileMatrix::write() {
            QJsonArray array;
            array.append((*this)(0, 0));  array.append((*this)(1, 0));  array.append((*this)(2, 0));  array.append((*this)(3, 0));
            array.append((*this)(0, 1));  array.append((*this)(1, 1));  array.append((*this)(2, 1));  array.append((*this)(3, 1));
            array.append((*this)(0, 2));  array.append((*this)(1, 2));  array.append((*this)(2, 2));  array.append((*this)(3, 2));
            array.append((*this)(0, 3));  array.append((*this)(1, 3));  array.append((*this)(2, 3));  array.append((*this)(3, 3));

            return array;
        }

        void TileMatrix::read(const QJsonValue& object) {
            if (!required(object, QJsonValue::Array))
                throw ThreedTilesParseException("transform is required");

            (*this)(0, 0) = object[0].toDouble();  (*this)(1, 0) = object[1].toDouble();  (*this)(2, 0) = object[2].toDouble();  (*this)(3, 0) = object[3].toDouble();
            (*this)(0, 1) = object[4].toDouble();  (*this)(1, 1) = object[5].toDouble();  (*this)(2, 1) = object[6].toDouble();  (*this)(3, 1) = object[7].toDouble();
            (*this)(0, 2) = object[8].toDouble();  (*this)(1, 2) = object[9].toDouble();  (*this)(2, 2) = object[10].toDouble();  (*this)(3, 2) = object[11].toDouble();
            (*this)(0, 3) = object[12].toDouble();  (*this)(1, 3) = object[13].toDouble();  (*this)(2, 3) = object[14].toDouble();  (*this)(3, 3) = object[15].toDouble();
        }


        void Refine::read(const QJsonValue& object) {
            if (!required(object, QJsonValue::String))
                throw ThreedTilesParseException("refine is required");
            if (object == "ADD") {
                type_ = ADD;
            }
            else if (object == "REPLACE") {
                type_ = REPLACE;
            }
            else {
                throw ThreedTilesParseException("refine is not add neither nor replace");
            }
        }
        QJsonValue Refine::write() {
            return QJsonValue(toString());
        }
        QString Refine::toString() const {
            return typeMeta.valueToKey(type_);
        }


        ContentTile::ContentTile() {
        }

        QJsonValue ContentTile::write() {
            QJsonObject object;
            if (boundingVolume != nullptr)
                object["boundingVolume"] = boundingVolume->write();
            object["uri"] = uri;
            return object;
        }

        void ContentTile::read(const QJsonValue& object) {

            if (object.isNull())
                return;

            if (required(object["boundingVolume"], QJsonValue::Object))
                boundingVolume = BoundingVolume::create(object["boundingVolume"]);

            if (!required(object["uri"], QJsonValue::String))
                throw ThreedTilesParseException("uri is required");
            uri = object["uri"].toString();
        }

        RootTile::RootTile() {
            children.clear();
        }
        QJsonValue RootTile::write() {
            QMetaEnum metaState = QMetaEnum::fromType<Refine::RefineType>();
            QJsonObject object;
            object["boundingVolume"] = boundingVolume->write();
            object["geometricError"] = geometricError;

            if (refine != nullptr)
                object["refine"] = refine->write();

            if (content != nullptr) {
                object["content"] = content->write();
            }

            if (children.size() != 0) {
                QJsonArray childrenValue;
                for (auto& child : children) {
                    childrenValue.append(child->write());
                }
                object["children"] = childrenValue;
            }

            return object;
        }

        void RootTile::read(const QJsonValue& object) {
            if (object.isNull() || object.isUndefined())
                return;

            if (!required(object["boundingVolume"], QJsonValue::Object))
                throw ThreedTilesParseException("boundingVolume is required");
            boundingVolume = BoundingVolume::create(object["boundingVolume"]);

            if (!required(object["geometricError"], QJsonValue::Double))
                throw ThreedTilesParseException("geometricError is required");
            geometricError = object["geometricError"].toDouble();

            if (required(object["transform"], QJsonValue::Array)) {
                transform = TileMatrix::TileMatrixPtr::create();
                transform->read(object["transform"]);
            }


            if (required(object["content"], QJsonValue::Object)) {
                content = ContentTile::ContentTilePtr::create();
                content->read(object["content"]);
            }


            if (required(object["refine"], QJsonValue::String)) {
                refine = Refine::RefinePtr::create();
                refine->read(object["refine"]);
            }


            if (required(object["children"], QJsonValue::Array)) {
                QJsonArray values = object["children"].toArray();
                for (const auto& value : values) {
                    RootTilePtr r = QSharedPointer<RootTile>::create();
                    r->read(value);
                    children.append(r);
                }
            }
        }

        BaseTile::BaseTile() {

        }

        QJsonValue BaseTile::write() {
            QJsonObject object;
            object[asset->typeName()] = asset->write();
            object["geometricError"] = geometricError;
            object[root->typeName()] = root->write();
            return object;
        }

        void BaseTile::read(const QJsonValue& object) {
            if (!object.isObject())
                throw ThreedTilesParseException("object is not Json Object");

            if (required(object["asset"], QJsonValue::Object)) {
                asset = AssetProperties::AssetPropertiesPtr::create();
                asset->read(object["asset"]);
            }

            QJsonValue geometricErrorValue = object["geometricError"];
            if (!required(geometricErrorValue, QJsonValue::Double))
                throw ThreedTilesParseException("geometricError is required");
            geometricError = geometricErrorValue.toDouble();

            if (required(object["root"], QJsonValue::Object)) {
                root = RootTile::RootTilePtr::create();
                root->read(object["root"]);
            }
        }

}
