<?php

// SQLiteEngine.php
//
// A PHP-based implementation of SqliteDB and SqliteVM class using PDO instead
// of the internal SWIG-wrapped SQLite classes in Oem
//
// This avoids any access violation issues should our test runner happen to
// be mis-using any classes in the SWIG wrapper.

if (!defined('SQLITE_ROW')) {
    define('SQLITE_ROW', 100);
}

if (!extension_loaded("SQLitePhpApi"))
{
    //echo "SQLitePhpApi not loaded. Using PHP implementation";

    class SqliteException extends Exception
    {
    }

    class CantOpenDbException extends SqliteException
    {
    }
    
    class SqliteDB
    {
        private $dbh;
        private $dbName;

        public function __construct()
        {
            $this->dbh = NULL;
            $this->dbName = "";
        }
        
        public function __destruct()
        {
            $this->Close();
        }

        public function Close()
        {
            if ($this->dbh != NULL)
            {
                $this->dbh = NULL;
            }
        }
        
        public function GetHandle() { return $this->dbh; }
        
        public function Open($dbName)
        {
            $this->Close();
            $this->dbh = new PDO("sqlite:$dbName");
            $this->dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
            $this->dbName = $dbName;
        }
        
        public function GetName() { return $this->dbName; }
        
        public function GenerateDatabase($dumpFilePath, $dbPath)
        {
            if (php_sapi_name() == 'cli')
                echo "Generating database from dump: $dumpFilePath\n";
            else
                echo "Generating database from dump: $dumpFilePath<br/>";
            $pdo = new PDO("sqlite:$dbPath");
            $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
            $sql = file_get_contents($dumpFilePath);
            $pdo->exec($sql);
            $pdo = NULL;
        }
        
        public function DumpDatabase($path)
        {
            throw new Exception("Creating database dumps not implemented. Please run this script with the php_SQLitePhpApi extension loaded");
        }
    }
    
    class SqliteVM
    {
        private $db;
        private $currentStmt;
        private $currentRow;
        private $resultLOB;
        
        private $nameIndexMap;

        public function __construct($db)
        {
            $this->db = $db->GetHandle();
            $this->currentStmt = NULL;
            $this->currentRow = NULL;
            $this->nameIndexMap = array();
        }
        
        public function Execute($query)
        {
            /*
            if (php_sapi_name() == 'cli')
                echo "Executing: $query\n";
            else
                echo "Executing: $query<br/>";
            */
            if ($this->currentStmt != NULL)
            {
                $this->currentStmt->closeCursor();
                $this->currentStmt = NULL;
            }
            $this->nameIndexMap = array();
            $this->currentStmt = $this->db->prepare($query);
            try
            {
                if ($this->currentStmt === FALSE)
                {
                    if (php_sapi_name() == 'cli')
                        echo "ERROR: Failed to prepare query: $query\n";
                    else
                        echo "ERROR: Failed to prepare query: $query<br/>";
                    return -1;
                }
                if (!$this->currentStmt->execute())
                {
                    if (php_sapi_name() == 'cli')
                        echo "ERROR: Failed to execute query: $query\n";
                    else
                        echo "ERROR: Failed to execute query: $query<br/>";
                    return -1;
                }
            }
            catch (PDOException $e)
            {
                return -1;
            }
            //Compile name index map for the purposes of column binding below
            for ($i = 0; $i < $this->currentStmt->columnCount(); $i++)
            {
                try {
                    $col = $this->currentStmt->getColumnMeta($i);
                    if ($col === FALSE)
                        continue;
                } catch (PDOException $e) {
                    continue;
                }
                if (array_key_exists("name", $col)) 
                {
                    $colName = $col["name"];
                    $this->nameIndexMap[$colName] = $i;
                }
            }
            
            //Bind columns, paying special attention to bind Result as BLOB if found
            $this->currentRow = array();
            foreach ($this->nameIndexMap as $colName => $index)
            {
                if ($colName == "Result") {
                    $this->currentStmt->bindColumn(($index+1), $this->currentRow[$colName], PDO::PARAM_LOB);
                    //echo "Bound $colName as BLOB<br/>";
                } else {
                    $this->currentStmt->bindColumn(($index+1), $this->currentRow[$colName]);
                }
            }
            
            try
            {
                $res = $this->currentStmt->fetch();
                if ($res === FALSE)
                    return -1;
            }
            catch (PDOException $e)
            {
                return -1;
            }
            return SQLITE_ROW;
        }
        
        public function GetString($name)
        {
            if (array_key_exists($name, $this->currentRow))
                return $this->currentRow[$name];
            return "";
        }
        
        public function GetBlob($name)
        {
            $str = $this->currentRow[$name];
            //SqliteBlob in C++ has a MgByteReader-style interface. Since
            //in PHP if it quacks like a duck ... we'll give them an actual MgByteReader
            $source = new MgByteSource($str, strlen($str));
            return $source->GetReader();
        }
        
        public function NextRow()
        {
            $this->currentRow = $this->currentStmt->fetch();
            if ($this->currentRow === FALSE)
                return -1;
            return SQLITE_ROW;
        }
        
        public function SqlFinalize()
        {
            if ($this->currentStmt != NULL)
            {
                $this->currentStmt->closeCursor();
                $this->currentStmt = NULL;
            }
        }
    }
}

?>