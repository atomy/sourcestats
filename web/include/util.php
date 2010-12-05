<?php

require_once "config_inc.php";
require_once "DatedRequest.php";

class Util {
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
            if(isset($_REQUEST['modname']) && false) { // do not use, just 1
                if(self::isModNameValid($_REQUEST['modname'])) {
                    $modname = $_REQUEST['modname'];
                }
            }            
            
            return new DatedRequest($day, $month, $year, $type, $modname);
    }
    
    public function isModNameValid($modname) {
        return true; // TODO
    }
}