<?php

//This class defines a Result object. Each Result object contains the data returned from an operation,
//content/mime type, and the httpStatusCode(http requests only)
class Result
{
    private $resultData;
    private $contentType;
    private $httpStatusCode;

    public function __construct($resultData="", $contentType="", $httpStatusCode="")
    {
        $this->resultData=$resultData;
        $this->contentType=$contentType;
        $this->httpStatusCode=$httpStatusCode;
    }

    public function SetResultData($newData)
    {
        $this->resultData=$newData;
    }

    public function SetContentType($newContentType)
    {
        $this->contentType=$newContentType;
    }

    public function SetHttpStatusCode($newHttpStatusCode)
    {
        $this->httpStatusCode=$newHttpStatusCode;
    }

    public function GetResultData()
    {
        return $this->resultData;
    }

    public function GetContentType()
    {
        return $this->contentType;
    }

    public function GetHttpStatusCode()
    {
        return $this->httpStatusCode;
    }
}
?>
