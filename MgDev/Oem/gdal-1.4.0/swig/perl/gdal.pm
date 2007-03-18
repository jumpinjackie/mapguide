# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.
package gdal;
require Exporter;
require DynaLoader;
@ISA = qw(Exporter DynaLoader);
package gdalc;
bootstrap gdal;
package gdal;
@EXPORT = qw( );

# ---------- BASE METHODS -------------

package gdal;

sub TIEHASH {
    my ($classname,$obj) = @_;
    return bless $obj, $classname;
}

sub CLEAR { }

sub FIRSTKEY { }

sub NEXTKEY { }

sub FETCH {
    my ($self,$field) = @_;
    my $member_func = "swig_${field}_get";
    $self->$member_func();
}

sub STORE {
    my ($self,$field,$newval) = @_;
    my $member_func = "swig_${field}_set";
    $self->$member_func($newval);
}

sub this {
    my $ptr = shift;
    return tied(%$ptr);
}


# ------- FUNCTION WRAPPERS --------

package gdal;

*UseExceptions = *gdalc::UseExceptions;
*DontUseExceptions = *gdalc::DontUseExceptions;
*Debug = *gdalc::Debug;
*Error = *gdalc::Error;
*PushErrorHandler = *gdalc::PushErrorHandler;
*PopErrorHandler = *gdalc::PopErrorHandler;
*ErrorReset = *gdalc::ErrorReset;
*GetLastErrorNo = *gdalc::GetLastErrorNo;
*GetLastErrorType = *gdalc::GetLastErrorType;
*GetLastErrorMsg = *gdalc::GetLastErrorMsg;
*PushFinderLocation = *gdalc::PushFinderLocation;
*PopFinderLocation = *gdalc::PopFinderLocation;
*FinderClean = *gdalc::FinderClean;
*FindFile = *gdalc::FindFile;
*SetConfigOption = *gdalc::SetConfigOption;
*GetConfigOption = *gdalc::GetConfigOption;
*CPLBinaryToHex = *gdalc::CPLBinaryToHex;
*CPLHexToBinary = *gdalc::CPLHexToBinary;
*GDAL_GCP_GCPX_get = *gdalc::GDAL_GCP_GCPX_get;
*GDAL_GCP_GCPX_set = *gdalc::GDAL_GCP_GCPX_set;
*GDAL_GCP_GCPY_get = *gdalc::GDAL_GCP_GCPY_get;
*GDAL_GCP_GCPY_set = *gdalc::GDAL_GCP_GCPY_set;
*GDAL_GCP_GCPZ_get = *gdalc::GDAL_GCP_GCPZ_get;
*GDAL_GCP_GCPZ_set = *gdalc::GDAL_GCP_GCPZ_set;
*GDAL_GCP_GCPPixel_get = *gdalc::GDAL_GCP_GCPPixel_get;
*GDAL_GCP_GCPPixel_set = *gdalc::GDAL_GCP_GCPPixel_set;
*GDAL_GCP_GCPLine_get = *gdalc::GDAL_GCP_GCPLine_get;
*GDAL_GCP_GCPLine_set = *gdalc::GDAL_GCP_GCPLine_set;
*GDAL_GCP_Info_get = *gdalc::GDAL_GCP_Info_get;
*GDAL_GCP_Info_set = *gdalc::GDAL_GCP_Info_set;
*GDAL_GCP_Id_get = *gdalc::GDAL_GCP_Id_get;
*GDAL_GCP_Id_set = *gdalc::GDAL_GCP_Id_set;
*GDAL_GCP_get_GCPX = *gdalc::GDAL_GCP_get_GCPX;
*GDAL_GCP_set_GCPX = *gdalc::GDAL_GCP_set_GCPX;
*GDAL_GCP_get_GCPY = *gdalc::GDAL_GCP_get_GCPY;
*GDAL_GCP_set_GCPY = *gdalc::GDAL_GCP_set_GCPY;
*GDAL_GCP_get_GCPZ = *gdalc::GDAL_GCP_get_GCPZ;
*GDAL_GCP_set_GCPZ = *gdalc::GDAL_GCP_set_GCPZ;
*GDAL_GCP_get_GCPPixel = *gdalc::GDAL_GCP_get_GCPPixel;
*GDAL_GCP_set_GCPPixel = *gdalc::GDAL_GCP_set_GCPPixel;
*GDAL_GCP_get_GCPLine = *gdalc::GDAL_GCP_get_GCPLine;
*GDAL_GCP_set_GCPLine = *gdalc::GDAL_GCP_set_GCPLine;
*GDAL_GCP_get_Info = *gdalc::GDAL_GCP_get_Info;
*GDAL_GCP_set_Info = *gdalc::GDAL_GCP_set_Info;
*GDAL_GCP_get_Id = *gdalc::GDAL_GCP_get_Id;
*GDAL_GCP_set_Id = *gdalc::GDAL_GCP_set_Id;
*GCPsToGeoTransform = *gdalc::GCPsToGeoTransform;
*AllRegister = *gdalc::AllRegister;
*GetCacheMax = *gdalc::GetCacheMax;
*SetCacheMax = *gdalc::SetCacheMax;
*GetCacheUsed = *gdalc::GetCacheUsed;
*GetDataTypeSize = *gdalc::GetDataTypeSize;
*DataTypeIsComplex = *gdalc::DataTypeIsComplex;
*GetDataTypeName = *gdalc::GetDataTypeName;
*GetDataTypeByName = *gdalc::GetDataTypeByName;
*GetColorInterpretationName = *gdalc::GetColorInterpretationName;
*GetPaletteInterpretationName = *gdalc::GetPaletteInterpretationName;
*DecToDMS = *gdalc::DecToDMS;
*PackedDMSToDec = *gdalc::PackedDMSToDec;
*DecToPackedDMS = *gdalc::DecToPackedDMS;
*ParseXMLString = *gdalc::ParseXMLString;
*SerializeXMLTree = *gdalc::SerializeXMLTree;
*GetDriverCount = *gdalc::GetDriverCount;
*GetDriverByName = *gdalc::GetDriverByName;
*GetDriver = *gdalc::GetDriver;
*Open = *gdalc::Open;
*OpenShared = *gdalc::OpenShared;
*AutoCreateWarpedVRT = *gdalc::AutoCreateWarpedVRT;

############# Class : gdal::MajorObject ##############

package gdal::MajorObject;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( gdal );
%OWNER = ();
*GetDescription = *gdalc::MajorObject_GetDescription;
*SetDescription = *gdalc::MajorObject_SetDescription;
*GetMetadata = *gdalc::MajorObject_GetMetadata;
*SetMetadata = *gdalc::MajorObject_SetMetadata;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : gdal::Driver ##############

package gdal::Driver;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( gdal::MajorObject gdal );
%OWNER = ();
%ITERATORS = ();
*swig_ShortName_get = *gdalc::Driver_ShortName_get;
*swig_ShortName_set = *gdalc::Driver_ShortName_set;
*swig_LongName_get = *gdalc::Driver_LongName_get;
*swig_LongName_set = *gdalc::Driver_LongName_set;
*swig_HelpTopic_get = *gdalc::Driver_HelpTopic_get;
*swig_HelpTopic_set = *gdalc::Driver_HelpTopic_set;
*Create = *gdalc::Driver_Create;
*CreateCopy = *gdalc::Driver_CreateCopy;
*Delete = *gdalc::Driver_Delete;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : gdal::GCP ##############

package gdal::GCP;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( gdal );
%OWNER = ();
%ITERATORS = ();
*swig_GCPX_get = *gdalc::GCP_GCPX_get;
*swig_GCPX_set = *gdalc::GCP_GCPX_set;
*swig_GCPY_get = *gdalc::GCP_GCPY_get;
*swig_GCPY_set = *gdalc::GCP_GCPY_set;
*swig_GCPZ_get = *gdalc::GCP_GCPZ_get;
*swig_GCPZ_set = *gdalc::GCP_GCPZ_set;
*swig_GCPPixel_get = *gdalc::GCP_GCPPixel_get;
*swig_GCPPixel_set = *gdalc::GCP_GCPPixel_set;
*swig_GCPLine_get = *gdalc::GCP_GCPLine_get;
*swig_GCPLine_set = *gdalc::GCP_GCPLine_set;
*swig_Info_get = *gdalc::GCP_Info_get;
*swig_Info_set = *gdalc::GCP_Info_set;
*swig_Id_get = *gdalc::GCP_Id_get;
*swig_Id_set = *gdalc::GCP_Id_set;
sub new {
    my $pkg = shift;
    my $self = gdalc::new_GCP(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        gdalc::delete_GCP($self);
        delete $OWNER{$self};
    }
}

sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : gdal::Dataset ##############

package gdal::Dataset;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( gdal::MajorObject gdal );
%OWNER = ();
%ITERATORS = ();
*swig_RasterXSize_get = *gdalc::Dataset_RasterXSize_get;
*swig_RasterXSize_set = *gdalc::Dataset_RasterXSize_set;
*swig_RasterYSize_get = *gdalc::Dataset_RasterYSize_get;
*swig_RasterYSize_set = *gdalc::Dataset_RasterYSize_set;
*swig_RasterCount_get = *gdalc::Dataset_RasterCount_get;
*swig_RasterCount_set = *gdalc::Dataset_RasterCount_set;
sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        gdalc::delete_Dataset($self);
        delete $OWNER{$self};
    }
}

*GetDriver = *gdalc::Dataset_GetDriver;
*GetRasterBand = *gdalc::Dataset_GetRasterBand;
*GetProjection = *gdalc::Dataset_GetProjection;
*GetProjectionRef = *gdalc::Dataset_GetProjectionRef;
*SetProjection = *gdalc::Dataset_SetProjection;
*GetGeoTransform = *gdalc::Dataset_GetGeoTransform;
*SetGeoTransform = *gdalc::Dataset_SetGeoTransform;
*BuildOverviews = *gdalc::Dataset_BuildOverviews;
*GetGCPCount = *gdalc::Dataset_GetGCPCount;
*GetGCPProjection = *gdalc::Dataset_GetGCPProjection;
*GetGCPs = *gdalc::Dataset_GetGCPs;
*SetGCPs = *gdalc::Dataset_SetGCPs;
*FlushCache = *gdalc::Dataset_FlushCache;
*AddBand = *gdalc::Dataset_AddBand;
*WriteRaster = *gdalc::Dataset_WriteRaster;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : gdal::Band ##############

package gdal::Band;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( gdal::MajorObject gdal );
%OWNER = ();
%ITERATORS = ();
*swig_XSize_get = *gdalc::Band_XSize_get;
*swig_XSize_set = *gdalc::Band_XSize_set;
*swig_YSize_get = *gdalc::Band_YSize_get;
*swig_YSize_set = *gdalc::Band_YSize_set;
*swig_DataType_get = *gdalc::Band_DataType_get;
*swig_DataType_set = *gdalc::Band_DataType_set;
*GetRasterColorInterpretation = *gdalc::Band_GetRasterColorInterpretation;
*SetRasterColorInterpretation = *gdalc::Band_SetRasterColorInterpretation;
*GetNoDataValue = *gdalc::Band_GetNoDataValue;
*SetNoDataValue = *gdalc::Band_SetNoDataValue;
*GetMinimum = *gdalc::Band_GetMinimum;
*GetMaximum = *gdalc::Band_GetMaximum;
*GetOffset = *gdalc::Band_GetOffset;
*GetScale = *gdalc::Band_GetScale;
*GetOverviewCount = *gdalc::Band_GetOverviewCount;
*GetOverview = *gdalc::Band_GetOverview;
*Checksum = *gdalc::Band_Checksum;
*ComputeRasterMinMax = *gdalc::Band_ComputeRasterMinMax;
*Fill = *gdalc::Band_Fill;
*ReadRaster = *gdalc::Band_ReadRaster;
*WriteRaster = *gdalc::Band_WriteRaster;
*FlushCache = *gdalc::Band_FlushCache;
*GetRasterColorTable = *gdalc::Band_GetRasterColorTable;
*SetRasterColorTable = *gdalc::Band_SetRasterColorTable;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


############# Class : gdal::ColorTable ##############

package gdal::ColorTable;
use vars qw(@ISA %OWNER %ITERATORS %BLESSEDMEMBERS);
@ISA = qw( gdal );
%OWNER = ();
%ITERATORS = ();
sub new {
    my $pkg = shift;
    my $self = gdalc::new_ColorTable(@_);
    bless $self, $pkg if defined($self);
}

sub DESTROY {
    return unless $_[0]->isa('HASH');
    my $self = tied(%{$_[0]});
    return unless defined $self;
    delete $ITERATORS{$self};
    if (exists $OWNER{$self}) {
        gdalc::delete_ColorTable($self);
        delete $OWNER{$self};
    }
}

*Clone = *gdalc::ColorTable_Clone;
*GetPaletteInterpretation = *gdalc::ColorTable_GetPaletteInterpretation;
*GetCount = *gdalc::ColorTable_GetCount;
*GetColorEntry = *gdalc::ColorTable_GetColorEntry;
*GetColorEntryAsRGB = *gdalc::ColorTable_GetColorEntryAsRGB;
*SetColorEntry = *gdalc::ColorTable_SetColorEntry;
sub DISOWN {
    my $self = shift;
    my $ptr = tied(%$self);
    delete $OWNER{$ptr};
}

sub ACQUIRE {
    my $self = shift;
    my $ptr = tied(%$self);
    $OWNER{$ptr} = 1;
}


# ------- VARIABLE STUBS --------

package gdal;

1;
