<?php

require_once "config_inc.php";
require_once "DatedRequest.php";
require_once "db2file_inc.php";

class Util {
    private static $availGames = array();
    
    public static function generateDatedRequest() {
        if(!isset($_REQUEST['type']))
            return null;
        
            $type = TYPE_DEFAULT;
            if(isset($_REQUEST['type']) && is_numeric($_REQUEST['type'])) {                            
                if($_REQUEST['type'] > TYPE_START && $_REQUEST['type'] < TYPE_LAST) {
                    $type = $_REQUEST['type'];
                }
            }
            
            $day = date("j"); // default
            if(isset($_REQUEST['day']) && is_numeric($_REQUEST['day'])) {
                if($_REQUEST['day'] > 0 && $_REQUEST['day'] <= 31) {
                    $day = $_REQUEST['day'];
                }
            }
            
            $month = date("n"); // default
            if(isset($_REQUEST['month']) && is_numeric($_REQUEST['month'])) {
                if($_REQUEST['month'] > 0 && $_REQUEST['month'] <= 12) {
                    $month = $_REQUEST['month'];
                }
            }
                    
            $year = date("Y"); // default 
            if(isset($_REQUEST['year']) && is_numeric($_REQUEST['year'])) {
                if($_REQUEST['year'] >= 2010 && $_REQUEST['year'] <= date("Y")) {
                    $year = $_REQUEST['year'];
                }
            }
            
            $modname = "dystopia"; // default
            if(isset($_REQUEST['modname'])) {
                if(self::isModNameValid($_REQUEST['modname'])) {
                    $modname = $_REQUEST['modname'];
                }
            }            
            
            return new DatedRequest($day, $month, $year, $type, $modname);
    }
    
    public function isModNameValid($modname) {
        self::getAvailGames();
            
		// only allow a-z, A-Z, 0-9 as gamename, preg_match == 0 if valid
		if (preg_match("/[^0-9A-Za-z]/", $modname) != 0) {
            return false;
		}
        else if (array_search($modname, self::$availGames, false) !== false) {
            return true;
        }
        else {
            return false;
        }
    }
    
    private function generateAvailableGameNames() {
        self::$availGames = DB2File::getAvailableGameNames();
    }
    
    public function getAvailGames() {
        if(count(self::$availGames) <= 0)
            self::generateAvailableGameNames();
            
        return self::$availGames;
    }
}