using System;
using System.Collections.Generic;
using System.Text;

namespace OSGeo.MapGuide.Viewer.Redlining
{
    public static class RedlineSchemaFactory
    {
        public const string SCHEMA_NAME = "MarkupSchema";
        public const string GEOM_NAME = "Geometry";
        public const string ID_NAME = "ID";
        public const string TEXT_NAME = "Text";
        public const string CLASS_NAME = "Markup";
        public const string SPATIAL_CONTEXT = "Default";

        public static MgDataPropertyDefinition CreateIDProperty()
        {
            MgDataPropertyDefinition id = new MgDataPropertyDefinition(ID_NAME);
            id.SetDataType(MgPropertyType.Int32);
            id.SetReadOnly(true);
            id.SetNullable(false);
            id.SetAutoGeneration(true);

            return id;
        }

        public static MgDataPropertyDefinition CreateTextProperty(int length)
        {
            MgDataPropertyDefinition text = new MgDataPropertyDefinition(TEXT_NAME);
            text.SetDataType(MgPropertyType.String);
            text.SetLength(length);

            return text;
        }

        public static MgGeometricPropertyDefinition CreateGeometryProperty(int geomType)
        {
            MgGeometricPropertyDefinition geom = new MgGeometricPropertyDefinition(GEOM_NAME);
            geom.SetGeometryTypes(geomType);
            geom.SetHasElevation(false);
            geom.SetHasMeasure(false);
            geom.SetReadOnly(false);
            geom.SetSpatialContextAssociation(SPATIAL_CONTEXT);

            return geom;
        }

        public static MgClassDefinition CreateClass(int geomType)
        {
            MgClassDefinition cls = new MgClassDefinition();
            cls.SetName(CLASS_NAME);
            MgPropertyDefinitionCollection props = cls.GetProperties();
            MgPropertyDefinitionCollection idProps = cls.GetIdentityProperties();

            MgDataPropertyDefinition id = CreateIDProperty();
            MgDataPropertyDefinition text = CreateTextProperty(255);
            MgGeometricPropertyDefinition geom = CreateGeometryProperty(geomType);

            props.Add(id);
            props.Add(text);
            props.Add(geom);

            idProps.Add(id);
            cls.SetDefaultGeometryPropertyName(GEOM_NAME);

            return cls;
        }

        public static MgFeatureSchema CreateSchema(int geomType)
        {
            MgFeatureSchema schema = new MgFeatureSchema();
            schema.SetName("MarkupSchema");
            if (geomType == -1)
                geomType = MgFeatureGeometricType.Point | MgFeatureGeometricType.Curve | MgFeatureGeometricType.Surface;

            MgClassDefinitionCollection classes = schema.GetClasses();
            MgClassDefinition cls = CreateClass(geomType);

            classes.Add(cls);

            return schema;
        }
    }
}
