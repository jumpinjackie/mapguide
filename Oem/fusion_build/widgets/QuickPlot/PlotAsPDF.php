<?php
    require_once("../../lib/tcpdf/config/lang/eng.php");
    require_once("../../lib/tcpdf/tcpdf.php");

    $host = $_SERVER["SERVER_NAME"];
    $protocol = (isset($_SERVER["HTTPS"]) && $_SERVER["HTTPS"] != "off") ? "https://" : "http://"; 
    $path = $_SERVER["PHP_SELF"];
    $port = $_SERVER["SERVER_PORT"];
    $generatePage = "/GeneratePicture.php?";
    $pathString = implode('/',explode('/', $path,-1));

    // POST params
    // Title
    $title = htmlspecialchars( $_POST["{field:title}"] );
    // Sub title
    $subTitle = htmlspecialchars( $_POST["{field:sub_title}"] );
    // DPI
    if( $_POST["dpi"] && trim($_POST["dpi"]) != "" ) 
    {
        $printDpi = intval($_POST["dpi"]);
    }
    else
    {
        $printDpi = 96;
    }
    // Paper size
    if( $_POST["paperSize"] && trim($_POST["paperSize"]) != "" )
    {
        $sizeArray = explode(",", $_POST["paperSize"]);
        $paperType = $sizeArray[2];
    }
    else
    {
        $paperType = "A4";
    }
    // Orientation
    if( $_POST["orientation"] && trim($_POST["orientation"]) != "" )
    {
        $orientation = $_POST["orientation"];
    }
    else
    {
        $orientation = "P";
    }
    // Margin
    if( $_POST["margin"] && trim($_POST["margin"]) != "")
    {
        //top,buttom,left,right
        $margin = explode(",", $_POST["margin"]);
    }
    else
    {
        // top,buttom,left,right,set default Margin = {0.5,0.5,0.5,0.5 inch}
        $margin = array("25.4","12.7","12.7","12.7");
    }
    // Scale
    if( $_POST["scaleDenominator"] && trim($_POST["scaleDenominator"]) != "" )
    {
        $scale = intval($_POST["scaleDenominator"]);
    }
    else 
    {
        $scale = 500;
    }
    // Print size
    if( $_POST["printSize"] && trim($_POST["printSize"]) != "" )
    {
        $printSizeArray = explode(",", $_POST["printSize"]);
        $printSize = new Size(ParseLocaleDouble($printSizeArray[0]), ParseLocaleDouble($printSizeArray[1]));
    }
    else
    {
        $printSize = new Size(0, 0);
    }

    // Construct the querysting which can be used to generate the Map image
    $query_string = "session_id=".$_POST['sessionId']."&map_name=".$_POST['mapName']."&print_size=".$_POST['printSize'].
                    "&print_dpi=".$_POST['dpi']."&box=".$_POST['box']."&normalized_box=".$_POST['normalizedBox'].
                    "&scale_denominator=".$_POST['scaleDenominator']."&rotation=".$_POST['rotation'];

    $filelocation = "";

    if("80" === $port)
    {
        $filelocation = $protocol.$host.$pathString.$generatePage.$query_string;
    }
    else
    {
        $filelocation = $protocol.$host.":".$port.$pathString.$generatePage.$query_string;
    }

    // Create new PDF document, the default "PDF_UNIT" value is "mm"
    $pdf = new TCPDF($orientation, PDF_UNIT, $paperType, true, "UTF-8", false);
    $font = "dejavusans";
    // Set margins  
    $pdf->SetMargins(0, 0, 0);
    $pdf->SetHeaderMargin(0);
    $pdf->SetFooterMargin(0);
    // Prevent adding page automatically
    $pdf->SetAutoPageBreak(false); 

    // Remove default header/footer
    $pdf->setPrintHeader(false);
    $pdf->setPrintFooter(false);
    // Set default font size
    $pdf->SetFont($font, "", 16, "", true);

    // Add a page
    $pdf->AddPage();
    
    // Draw Map first, so if the margin is not enough for the Text, the Text will be displayed about the image
    $pdf->Image($filelocation, $margin[2], $margin[0], $printSize->width, $printSize->height, "PNG", "", "", false, $printDpi, "", false, false, 1, false, false, false);

    // Draw Title
    DrawTitle();
    
    // Draw Extent coordinates
    DrawExtentCS();

    // Draw Scale
    DrawScale();
    
    // Draw declaration
    DrawDeclaration();
    
    // Close and output PDF document
    $pdf->Output($title.'.pdf', 'I');
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
    
    function ParseLocaleDouble($stringValue)
    {
        $lc = localeconv();
        $result = str_replace(".", $lc["decimal_point"], $stringValue);
        return doubleval($result);
    }

    //This function try to get a more elegant number for the scale bar display.
    //For example, convert 5.3 to 5, 5.5 to 6, 13 to 10, 230 to 200 and 1234 to 1200.
    //Basically no number will execced 9999 in scale bar display, however we support that situation
    //the minimum number for the return value is 0
    function GetRoundNumber($number)
    {
        $number = abs($number);
        $temp = $number = round($number);
        $len = 0;
        
        while($temp > 0)
        {
            $len++;
            $temp /= 10;
            $temp = floor($temp);
        }      
        
        //10,20,30,40,50,60,70,80,90
        if( 2 === $len )
        {
            $number = $number / 10;
            $number = round($number);
            $number = $number * 10;
        }
        
        //100,200,300,400,500,600,700,800,900
        if( $len >= 3 )
        {
            $number = $number / 100;
            $number = round($number);
            $number = $number * 100;
        }
        
        //else, just 1,2,3,4,5,6,7,8,9
        return $number; 
    }
    
    function DrawScale()
    {
        global $pdf,$margin,$font,$scale;
     
        $pageHeight = $pdf->getPageHeight();
        $paddingBottom = 7; $textPadding = 5; $fontSize = 6;             
        $style = array("width" => 0.4, "cap" => "butt", "join" => "miter", "dash" => 0, "color" => array(0, 0, 0));
        
        $start_x = ParseLocaleDouble($margin[2]);
        $start_y = $pageHeight - $paddingBottom;
        
        $lineMark_h = 3;
        $textMarkPadding = 1.0;
        
        //<editor-fold defaultstate="collapsed" desc="print the scale bar with meter">
        $unit = "m";
        $imageSpan = 20;            // the 20 is a suggested scale bar length
        $realSpan = $scale * 0.02;  // $imageSpan / 1000
        
        if($realSpan >= 1000)
        {
            $unit = "km";
            $realSpan /= 1000;
            $realSpan = GetRoundNumber($realSpan);
            $imageSpan = ($realSpan * 1000000)/$scale;
        }
        else 
        {
            $realSpan = GetRoundNumber($realSpan);
            $imageSpan = ($realSpan * 1000) / $scale;
        }
        
        $end_x = $start_x + $imageSpan;
        $end_y = $start_y;       
        $meterTextMark = $realSpan." ".$unit;
        
        $pdf->SetFont($font, "", $fontSize, "", true);
        $pdf->Line($start_x, $start_y, $end_x, $end_y, $style);
        $pdf->Line($start_x, $start_y, $start_x, $start_y - $lineMark_h, $style);
        $pdf->Line($end_x, $end_y, $end_x, $end_y - $lineMark_h, $style);
        $fontSize = 7; $textHeight = 4;
        $pdf->SetFont($font, "", $fontSize, "", true);
        $pdf->Text($start_x + $textMarkPadding, $start_y - $textHeight, $meterTextMark);
  
        $textStart_x = $end_x; 
        //</editor-fold>
        
        //<editor-fold defaultstate="collapsed" desc="print the scale bar with feet">
        $unit ="ft"; $aFeetPerMeter = 3.2808; $aFeetPerMile = 5280; $aMeterPerFeet = 0.3048;
        $imageSpan = 20; // the 20 is a suggested scale bar length, in "mm"
        $realSpan = ( ($scale * $imageSpan) / 1000 ) * $aFeetPerMeter;
        
        if($realSpan > $aFeetPerMile)
        {
            $unit = "mi";
            $realSpan /= $aFeetPerMile;
            $realSpan = GetRoundNumber($realSpan);
            $imageSpan = ( $realSpan * $aFeetPerMile * $aMeterPerFeet * 1000 ) / $scale;
        }
        else 
        {
            $realSpan = GetRoundNumber($realSpan);
            $imageSpan = ( $realSpan * $aMeterPerFeet * 1000 ) / $scale;
        }
        
        $end_x = $start_x + $imageSpan;
        $end_y = $start_y;
        
        $feetTextMark = $realSpan.' '.$unit;
        
        $pdf->Line($start_x, $start_y, $end_x, $end_y, $style);
        $pdf->Line($start_x, $start_y, $start_x, $start_y + $lineMark_h, $style);
        $pdf->Line($end_x, $end_y, $end_x, $end_y + $lineMark_h, $style);
        
        $pdf->SetFont($font, "", $fontSize, "", true);
        $pdf->Text($start_x + $textMarkPadding, $start_y + 1, $feetTextMark);
        //</editor-fold>
        
        //determine where to begin to print the absolute scale and date info
        if($end_x > $textStart_x)
        {
            $textStart_x = $end_x;
        }
        
        $textStart_x += $textPadding;
        
        //write the scale
        $fontSize = 8;
        $pdf->SetFont($font, "", $fontSize, "", true);
        $scaleText = "Scale 1:".$scale;
        $pdf->Text($textStart_x, $end_y + 0.2, $scaleText);
        //write the date
        $date = date("M/d/Y");
        $pdf->Text($textStart_x + 0.3, $end_y - 3.8, $date);  
    }

    function DrawDeclaration()
    {
        global $pdf, $font, $margin, $printSize;
    
        $declaration= $_POST["legalNotice"];
        //$declaration_w = $pdf->GetStringWidth($declaration,$font,9);
        $pdf->SetFont($font, "", 9, "", true);
        
        $bottomPadding = 2.5;
        
        //Sometimes the declaration is too short, less than 100 unit, we could set the cell width as the string length
        //so it will align to the right
        $SingleLineDeclarationWidth  = $pdf->GetStringWidth($declaration, $font, "", 9, false);
        $tolerance = 3;
        $w = 100;
        
        if( $SingleLineDeclarationWidth + $tolerance < $w )
        {
            $w = $SingleLineDeclarationWidth + $tolerance;
        }
        
        $h = 5;
        $border = 0; //no border
        $align = "L";//align left
        $tolerance = 2;
        $x = ParseLocaleDouble($margin[2]) + $printSize->width - $w + $tolerance;
        $cellTotalHeight = $pdf->getStringHeight($w,$declaration);
        $y = $pdf->getPageHeight() - $cellTotalHeight - $bottomPadding;

        $pdf->MultiCell($w, $h, $declaration, $border, $align, false, 0, $x , $y , true);
    }
    
    function DrawExtentCS()
    {
        global $pdf, $font, $margin, $printSize;
        
        if( $_POST["normalizedBox"] && trim($_POST["normalizedBox"]) != "" )
        {
            $fontSize = 9; $decimals = 6; $padding = 5; $textHeight = 5;
            
            $normalizedBox = $_POST["normalizedBox"];
            
            $extent_cs = explode(",",$normalizedBox);//2,3 ; 6,7
            $lefttop_cs = " x:".number_format($extent_cs[6], $decimals).", y:".number_format($extent_cs[7], $decimals)."   ";
            $rightbuttom_cs = " x:".number_format($extent_cs[2], $decimals).", y:".number_format($extent_cs[3], $decimals)."   ";
            $pdf->SetFont($font, "", $fontSize, "", true);

            //cell width
            $lt_cellwidth = $pdf->GetStringWidth($lefttop_cs,$font,$fontSize);
            $rb_cellwidth = $pdf->GetStringWidth($rightbuttom_cs,$font,$fontSize);
            //cell location
            $lefttop = array((ParseLocaleDouble($margin[2]) + $padding),(ParseLocaleDouble($margin[0]) + $padding));
            $rightbuttom = array((ParseLocaleDouble($margin[2]) + $printSize->width - $rb_cellwidth - $padding),(ParseLocaleDouble($margin[0]) + $printSize->height - $padding - $textHeight));

            $pdf->SetFillColor(255, 255, 255);

            $pdf->SetXY($lefttop[0], $lefttop[1], false);
            $pdf->Cell($lt_cellwidth, 0, $lefttop_cs, 1, 0, '', true, '', 0, false, 'T', 'M');

            $pdf->SetXY($rightbuttom[0], $rightbuttom[1], false);
            $pdf->Cell($rb_cellwidth, 0, $rightbuttom_cs, 1, 0, '', true, '', 0, false, 'T', 'M');
        }
    }
    
    function DrawTitle()
    {
        global $pdf, $margin, $title, $subTitle,$font;
        
        $html = '<div style="text-align:left"><span style="font-weight: bold; font-size: 18pt;">'.$title.'</span><br/><span>'.$subTitle.'</span></div>';
        
        //print title left position
        $titleWidth = $pdf->GetStringWidth($title, $font, "B", 18, false);
        $x = ($pdf->getPageWidth() - $titleWidth) / 2;
        if($x < 0.0)
        {
            $x = 0;
        }
        
        //print title top position
        $y = 5;
        if( $margin[0] > 0.0 )
        {
            $y = $margin[0] / 4;
        }
        
        // Print text using writeHTMLCell()
        $pdf->writeHTMLCell(0, 0, $x, $y, $html, 0, 1, false, true, "C", true);
    }
?>