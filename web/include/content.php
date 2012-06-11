<?php

require_once "include/config_inc.php";
require_once "include/util.php";
require_once "include/func_inc.php";
require_once "include/db2file_inc.php";

class Content
{
    public static function printWelcomeHeader() {
        ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
    <title>Source Mod Stats</title>
    <meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
</head>
        <?php
    }
    
    public static function printRequestedHeader(&$datedRequest) {
        ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
    <title>Source Mod Stats</title>
    <meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
    <script src="http://api.simile-widgets.org/timeplot/1.1/timeplot-api.js" type="text/javascript"></script>
    <script src="timeplot.php<?php echo "?modname=".$datedRequest->getModName()."&day=".$datedRequest->getDay()."&month=".$datedRequest->getMonth()."&year=".$datedRequest->getYear()."&type=".$datedRequest->getType();?>" type="text/javascript"></script>
</head>
        <?php
    }

    public static function printIntro() {
        ?>
<div id="header" style="text-align: center; margin-top: 20px;">
<a href="<?php echo $_SERVER['PHP_SELF'];?>"><img src="images/source_engine.png" alt="source engine logo" style="border: none; width: 325px; height: 100px;"/></a>
<h2 style="margin-top: 0px;">Mod Statistics</h2>
Plotting amount of players over time for a given Source-Mod (all times are in GMT+1)
<hr/>
        <?php
    }
    
    public static function printPage() {
        $datedRequest = Util::generateDatedRequest();
        
        if($datedRequest) {            
            self::printRequestedPage($datedRequest, $gameRequest);
        } else {
            self::printWelcomePage();
        }
    }
    
    public static function printRequestedPage(&$datedRequest, &$gameRequest) {        
        $success = DB2File::stats2File($datedRequest->getModName(), $datedRequest->getYear(), $datedRequest->getMonth(), $datedRequest->getDay(), $datedRequest->getType());
        
        if(!$success) {
            die("Sorry, no data available for the given timeframe! <a href=\"".$_SERVER['PHP_SELF']."\">back</a>");            
        }
            
        self::printRequestedHeader($datedRequest);
        self::printRequestedBody($datedRequest);
        self::printFooter();
    }
    
    public static function printWelcomePage() {
        self::printWelcomeHeader();
        self::printWelcomeBody();
        self::printFooter();
    }
    
    public static function printWelcomeBody() {
        ?>
<body>
    <?php self::printIntro();?>
<form action="#">
    <p>
    choose mod:
    <select name="modname">
        <?php
            $gamez = Util::getAvailGames();
            $i=0;
            foreach($gamez as $id=>$gameName) {                
                ?>                
                <option value="<?php echo $gameName;?>"><?php echo $gameName;?></option>
                <?php
            }
        ?>        
    </select>
    
    <br/>
    choose type:
    <select name="type">
        <option value="<?php echo TYPE_HOURLY;?>">hourly</option>
        <option value="<?php echo TYPE_DAILY;?>" selected="selected">daily</option>
        <option value="<?php echo TYPE_MONTHLY;?>">monthly</option>
    </select>    
    <br/>
    choose date:
    <select name="day">
        <?php
        $curDay = date("d");
        for($i=1; $i<=31; $i++) {        
            if($i == $curDay) {
        ?>
        <option value="<?php echo $i;?>" selected="selected"><?php echo $i;?></option>
        <?php
            } else {
        ?>
        <option value="<?php echo $i;?>"><?php echo $i;?></option>
        <?php
            }
        }
        ?>
    </select>
    <select name="month">
        <?php
        $curMonth = date("n");
        for($i=1; $i<=12; $i++) {        
            if($i == $curMonth) {
        ?>
        <option value="<?php echo $i;?>" selected="selected"><?php echo $i;?></option>
        <?php
            } else {
        ?>
        <option value="<?php echo $i;?>"><?php echo $i;?></option>
        <?php
            }
        }
        ?>
    </select>
    <select name="year">
        <?php
        $curYear = date("Y");
        for($i=2010; $i<=$curYear; $i++) {        
            if($i == $curYear) {
        ?>
        <option value="<?php echo $i;?>" selected="selected"><?php echo $i;?></option>
        <?php
            } else {
        ?>
        <option value="<?php echo $i;?>"><?php echo $i;?></option>
        <?php
            }
        }
        ?>
    </select>    
    <br/>
    <button>GO!</button>
    </p>
</form>

        <?php
    }
    
    
    public static function printFooter() {
        ?>
        <div style="width: 100px; float: right; font-size:0.8em; text-align: right;"><a href="mailto:atomy@jackinpoint.net">contact</a></div>
        <div style="width: 100px; float: left; font-size:0.8em; text-align: left;"><a href="index.php">Home</a></div>
        </body>
        </html>
        <?php
    }
    
    public static function printRequestedBody(&$datedRequest) {
        ?>
        <body onload="onLoad();" onresize="onResize();">            
        <?php self::printIntro();?>
        <div style="text-align: center;"><?php echo "<i>player statistics for mod </i><b>".$datedRequest->getModName()."</b><i> for the given timerange</i>";?></div>
        <div id="plotter" style="height: 300px;"></div>        
        <?php
    }
}
