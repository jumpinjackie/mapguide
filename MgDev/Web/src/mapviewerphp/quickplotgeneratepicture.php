<?php

    include 'common.php';
    include 'constants.php';
    
    $sessionID = "";
    $mapName   = "";
    $rotation  = 0.0;
    $printDpi  = 300;
    $scaleDenominator = 0;
    $captureBox;
    $normalizedCapture;
    $printSize;
    $paperSize;
    
    GetParameters();
    GenerateMap($printSize);
?>

<?php    
    function GetParameters()
    {
        global $sessionID, $mapName, $printDpi, $rotation, $paperSize, $captureBox, $printSize, $scaleDenominator, $normalizedCapture;
        $args = $_GET;
        if ($_SERVER["REQUEST_METHOD"] == "POST")
        {
            $args = $_POST;
        }
        
        // Not necessary to validate the parameters    	
        $sessionID   = $args["session_id"];
        $mapName     = $args["map_name"];
        $rotation    = floatval($args["rotation"]);
        $printDpi    = intval($args["print_dpi"]);

        $scaleDenominator = intval($args["scale_denominator"]);

        $array       = explode(",", $args["paper_size"]);
        $paperSize   = new Size(floatval($array[0]), floatval($array[1]));
        $printSize   = new Size($paperSize->width / 25.4 * $printDpi, $paperSize->height / 25.4 * $printDpi);
        
        $array       = explode(",", $args["box"]);
        $captureBox  = CreatePolygon($array);
        
        $array       = explode(",", $args["normalized_box"]);
        $normalizedCapture = CreatePolygon($array);
    }
    
    function CreatePolygon($coordinates)
    {
        $geometryFactory      = new MgGeometryFactory();
        $coordinateCollection = new MgCoordinateCollection();
        $linearRingCollection = new MgLinearRingCollection();
        
        for ($index = 0; $index < count($coordinates); ++$index)
        {
            $coordinate = $geometryFactory->CreateCoordinateXY(floatval($coordinates[$index]), floatval($coordinates[++$index]));
            $coordinateCollection->Add($coordinate);
        }
        
        $coordinateCollection->Add($geometryFactory->CreateCoordinateXY(floatval($coordinates[0]), floatval($coordinates[1])));
        
        $linearRingCollection = $geometryFactory->CreateLinearRing($coordinateCollection);
        $captureBox           = $geometryFactory->CreatePolygon($linearRingCollection, null);
        
        return $captureBox;
    }

    function GenerateMap($size)
    {
        global $sessionID, $mapName, $captureBox, $printSize, $normalizedCapture, $rotation, $scaleDenominator, $printDpi;
        
        $userInfo         = new MgUserInformation($sessionID);
        $siteConnection   = new MgSiteConnection();
        $siteConnection->Open($userInfo);
        $resourceService  = $siteConnection->CreateService(MgServiceType::ResourceService);
        $renderingService = $siteConnection->CreateService(MgServiceType::RenderingService);
        
        $map = new MgMap();
        $map->Open($resourceService, $mapName);
        
        $selection        = new MgSelection($map);
        
        // Calculate the generated picture size
        $envelope         = $captureBox->Envelope();
        $normalizedE      = $normalizedCapture->Envelope();
        $size1            = new Size($envelope->getWidth(), $envelope->getHeight());
        $size2            = new Size($normalizedE->getWidth(), $normalizedE->getHeight());
        $toSize           = new Size($size1->width / $size2->width * $size->width, $size1->height / $size2->height * $size->height);
        $center           = $captureBox->GetCentroid()->GetCoordinate();
        
        // Get the map agent url
        // Get the correct http protocal
        $mapAgent = "http";
        if ($_SERVER["HTTPS"] == "on")
        {
            $mapAgent .= "s";
        }
        // Get the correct port number
        // Just use the 127.0.0.1 specificly to point to localhost. Because the WebExtension will
        // be always on the same server with map agent. 
        $mapAgent .= "://127.0.0.1:" . $_SERVER["SERVER_PORT"];
        // Get the correct virtual directory
        $mapAgent .= substr($_SERVER["REQUEST_URI"], 0, strpos($_SERVER["REQUEST_URI"], "/", 1));
        $mapAgent .="/mapagent/mapagent.fcgi?VERSION=1.0.0&OPERATION=GETMAPIMAGE" .
                    "&SESSION=$sessionID" .  
                    "&MAPNAME=$mapName" .
                    "&FORMAT=PNG" .
                    "&SETVIEWCENTERX=" . $center->GetX() .
                    "&SETVIEWCENTERY=" . $center->GetY() .
                    "&SETVIEWSCALE=$scaleDenominator" . 
                    "&SETDISPLAYDPI=$printDpi" . 
                    "&SETDISPLAYWIDTH=$toSize->width" . 
                    "&SETDISPLAYHEIGHT=$toSize->height" . 
                    "&CLIP=0";

        $image = imagecreatefrompng($mapAgent);
        // Rotate the picture back to be normalized
        $normalizedImg = imagerotate($image, -$rotation, 0);
        // Free the original image
        imagedestroy($image);
        // Crop the normalized image
        $croppedImg = imagecreatetruecolor($size->width, $size->height);
        imagecopy($croppedImg, $normalizedImg, 0, 0, (imagesx($normalizedImg) - $size->width) / 2, (imagesy($normalizedImg) - $size->height) / 2, $size->width, $size->height);
        // Free the normalized image
        imagedestroy($normalizedImg);
        // Draw the north arrow on the map
        DrawNorthArrow($croppedImg);

        header ("Content-type: image/png"); 
        imagepng($croppedImg);
        imagedestroy($croppedImg);
    }
    
    function DrawNorthArrow($map)
    {
        global $paperSize, $rotation;
        
        // Load the north arrow image which has a 300 dpi resolution
        $na         = imagecreatefrompng("../viewerfiles/quickplotnortharrow.png");
        // Rotate the north arrow according to the capture rotation
        $transparent= imagecolortransparent($na);
        $rotatedNa  = imagerotate($na, -$rotation, $transparent);
        // Free the north arrow image
        imagedestroy($na);
        // Get the size of north arrow image
        $naWidth    = imagesx($rotatedNa);
        $naHeight   = imagesy($rotatedNa);
        // Get the map size
        $mapWidth   = imagesx($map);
        $mapHeight  = imagesy($map);
        // Get the logical resolution of map
        $resolution = $mapWidth * 25.4 / $paperSize->width;
        // On printed paper, north arrow is located at the right bottom corner with 6 MM margin
        $naRes      = 300;
        $naMargin   = 12;
        // Calculate the margin as pixels according to the resolutions
        $margin     = $resolution * $naMargin / 25.4;
        // Get the width of the north arrow on the map picture
        $drawWidth  = $naWidth * $resolution / $naRes;
        $drawHeight = $naHeight * $resolution / $naRes;
        // Draw the north arrow on the map picture
        imagecopyresized($map, $rotatedNa, $mapWidth - $drawWidth - $margin, $mapHeight - $drawHeight - $margin, 0, 0, $drawWidth, $drawHeight, $naWidth, $naHeight);
        // Free the north arrow image
        imagedestroy($rotatedNa); 
    }

?>

<?php    
    class Size
    {
        public $width;
        public $height;
        
        public function __construct($width, $height)
        {
            $this->width  = $width;
            $this->height = $height;
        }
    }
?>