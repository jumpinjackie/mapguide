<?php

class MarkupSchemaFactory
{
	static function CreateIDProperty()
	{
		$idProperty = new MgDataPropertyDefinition('ID');
		$idProperty->SetDataType(MgPropertyType::Int32);
		$idProperty->SetReadOnly(true);
		$idProperty->SetNullable(false);
		$idProperty->SetAutoGeneration(true);
		
		return $idProperty;
	}

	static function CreateTextProperty($length)
	{
		$textProperty = new MgDataPropertyDefinition('Text');
		$textProperty->SetDataType(MgPropertyType::String);
		$textProperty->SetLength($length);
		
		return $textProperty;
	}

	static function CreateGeometryProperty($geomType)
	{
		$geometryProperty = new MgGeometricPropertyDefinition('Geometry');
		$geometryProperty->SetGeometryTypes($geomType);
		$geometryProperty->SetHasElevation(false);
		$geometryProperty->SetHasMeasure(false);
		$geometryProperty->SetReadOnly(false);
		$geometryProperty->SetSpatialContextAssociation('Default');
		
		return $geometryProperty;
	}

	static function CreateMarkupClass()
	{
		$markupClass = new MgClassDefinition();
		$markupClass->SetName('Markup');
		$properties = $markupClass->GetProperties();
		
		$idProperty = MarkupSchemaFactory::CreateIDProperty();
		$properties->Add($idProperty);
		$properties->Add(MarkupSchemaFactory::CreateTextProperty(255));
		$properties->Add(MarkupSchemaFactory::CreateGeometryProperty(MgFeatureGeometricType::Point | MgFeatureGeometricType::Curve | MgFeatureGeometricType::Surface));
		
		$markupClass->GetIdentityProperties()->Add($idProperty);
		$markupClass->SetDefaultGeometryPropertyName('Geometry');
		
		return $markupClass;
	}

	static function CreateMarkupSchema()
	{
		$markupSchema = new MgFeatureSchema();
		$markupSchema->SetName('MarkupSchema');
		
		$markupSchema->GetClasses()->Add(MarkupSchemaFactory::CreateMarkupClass());
		
		return $markupSchema;
	}	
}

?>
