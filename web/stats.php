<?php

require_once "include/config_inc.php";
require_once "include/func_inc.php";
require_once "include/db2file_inc.php";

$defaultYear = date('Y');
$defaultMonth = date('n');
$defaultDay = date('j');
$defaultType = TYPE_DAILY;

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
	<script src="include/timeplot/timeplot-api.js?local" type="text/javascript"></script>
	<script src="timeplot.php<?php echo "?day=".$choosedDay."&month=".$choosedMonth."&year=".$choosedYear."&type=".$choosedType;?>" type="text/javascript"></script>
	<head>
  </head>
  <body onload="onLoad();" onresize="onResize();">
  <div id="statsselect">
	<form action="index.php" method="get">
      <select name="year">	  
	  <?php	 
	    foreach( $years as $year )
		{
		echo $year."\n";
		?>
			<option value="<?php echo $year;?>" <?php echo ($choosedYear == $year) ? "selected=\"selected\"" : "";?>><?php echo $year;?></option>  
		<?php
		}        
	  ?>
	  </select>	
      <select name="month">
	  <?php	  
	    foreach( $months as $month )
		{
		?>
			<option value="<?php echo $month;?>" <?php echo ($choosedMonth == $month) ? "selected=\"selected\"" : "";?>><?php echo $month;?></option>  
		<?php
		}        
	  ?>    
	  </select>
      <select name="day">
	  <?php	  
	    foreach( $days as $day )
		{
		?>
			<option value="<?php echo $day;?>" <?php echo ($choosedDay == $day) ? "selected=\"selected\"" : "";?>><?php echo $day;?></option>  
		<?php
		}        
	  ?>      
	  </select>	  
      <select name="type">
	  <?php	  
	    foreach( $types as $typeID=>$typeName)
		{
		?>
			<option value="<?php echo $typeID;?>" <?php echo ($choosedType == $typeID) ? "selected=\"selected\"" : "";?>><?php echo $typeName;?></option>  
		<?php
		}        
	  ?>      
	  </select>	  	  
	  <input type="submit" value="submit"/>
    </form>
  </div> <!-- statsselect/ -->
  <div id="printchoose">
	<center><h3><b>'<?php echo $choosenTypeName;?>'</b> Stats of Game <b>'Dystopia'</b> for Date: <i><?php echo date('j. (D) M Y ', $choosedTimeStamp);?></i></h2></center>
  </div> <!-- printchoose/ -->
  <div id="hourly" style="height: 300px;"></div>
  </body>
</html>
