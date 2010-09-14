<?php

require_once "include/config_inc.php";
require_once "include/func_inc.php";
require_once "include/db2file_inc.php";

$defaultYear = date('Y');
$defaultMonth = date('n');
$defaultDay = date('j');
$defaultType = TYPE_HOURLY;

if ( isset($_REQUEST['day']) && isset($_REQUEST['month']) && isset($_REQUEST['year']) && isset($_REQUEST['type']))
{
	if (!IsDateValid($_REQUEST['day'], $_REQUEST['month'], $_REQUEST['year']))
		die("invalid date");
		
	if (!IsTypeValid($_REQUEST['type']))
		die("invalid type");		
		
	$choosedYear = $_REQUEST['year'];
	$choosedMonth = $_REQUEST['month'];
	$choosedDay = $_REQUEST['day'];
	$choosedType = $_REQUEST['type'];
}
else
{
	$choosedYear = $defaultYear;
	$choosedMonth = $defaultMonth;
	$choosedDay = $defaultDay;
	$choosedType = $defaultType;
}

$choosedTimeStamp = mktime(0, 0, 0, $choosedMonth, $choosedDay, $choosedYear);
$dblink = SetupDB();
	
if (!DayStats2File($dblink, $choosedYear, $choosedMonth, $choosedDay, $choosedType))
	die("no data or error!");

$years = $months = $days = $types = array();
BuildChooseableArrays($dblink, &$days, &$months, &$years, &$types);
$choosenTypeName = $types[$choosedType];
?>

<html>
    <script src="http://api.simile-widgets.org/timeplot/1.1/timeplot-api.js" type="text/javascript"></script>
	<script src="timeplot.php<? print "?day=".$choosedDay."&month=".$choosedMonth."&year=".$choosedYear."&type=".$choosedType;?>" type="text/javascript"></script>
  <head>
  </head>
  <body onload="onLoad();" onresize="onResize();">
  <div id="statsselect">
	<form action="index.php" method="get">
      <select name="year">	  
	  <?
	 
	    foreach( $years as $year )
		{
		?>
			<option value="<?=$year?>" <? echo ($choosedYear == $year) ? "selected=\"selected\"" : "";?>><?=$year?></option>  
		<?
		}        
	  ?>
	  </select>	
      <select name="month">
	  <?
	  
	    foreach( $months as $month )
		{
		?>
			<option value="<?=$month?>" <? echo ($choosedMonth == $month) ? "selected=\"selected\"" : "";?>><?=$month?></option>  
		<?
		}        
	  ?>    
	  </select>
      <select name="day">
	  <?
	  
	    foreach( $days as $day )
		{
		?>
			<option value="<?=$day?>" <? echo ($choosedDay == $day) ? "selected=\"selected\"" : "";?>><?=$day?></option>  
		<?
		}        
	  ?>      
	  </select>	  
      <select name="type">
	  <?
	  
	    foreach( $types as $typeID=>$typeName)
		{
		?>
			<option value="<?=$typeID?>" <? echo ($choosedType == $typeID) ? "selected=\"selected\"" : "";?>><?=$typeName?></option>  
		<?
		}        
	  ?>      
	  </select>	  	  
	  <input type="submit" value="submit"/>
    </form>
  </div> <!-- statsselect/ -->
  <div id="printchoose">
	<center><h3><b>'<?=$choosenTypeName?>'</b> Stats of Game <b>'Dystopia'</b> for Date: <i><?=date('j. (D) M Y ', $choosedTimeStamp)?></i></h2></center>
  </div> <!-- printchoose/ -->
  <div id="hourly" style="height: 300px;"></div>
  </body>
</html>
