<?php
require_once "include/config_inc.php";
require_once "include/func_inc.php";

define( "DEBUG", false );


function getLastDayOfMonth($year, $month)
{	
	for ($day = 28; $day < 33; $day++) 
	{
		if (!checkdate($month, $day, $year)) 
			return $day-1;
	}	
	//die("getLastDayOfMonth() errored - year: $year month: $month<br/>");
}

class DB2File {
public static function stats2File($year, $month, $day, $type, $force = false)
{
	$dblink =& setupDB();
	
	if($type == TYPE_HOURLY)
	{
		if($year == date('Y') && $month == date('m') && $day == date('j')) {
			$force = true;
		}
		$fname = "stats/hourly_".$year."-".$month."-".$day.".txt";
		$_startTime = mktime(0, 0, 0, $month, $day, $year);
		$_endTime = $_startTime + 3600*24;
	}
	else if ($type == TYPE_DAILY)
	{
		if($year == date('Y') && $month == date('m')) {
			$force = true;
		}		
		$fname = "stats/daily_".$year."-".$month.".txt";
		$_startTime = mktime(1, 1, 1, $month, 1, $year);
		$_endTime = $_startTime + 3600*24*getLastDayOfMonth($year, 12);
	}
	else if ($type == TYPE_MONTHLY)
	{
		if($year == date('Y')) {
			$force = true;
		}
		$fname = "stats/monthly_".$year."-".$month.".txt";
		$_startTime = mktime(1, 1, 1, 1, 1, $year);
		
		if(date("L") == 1)
			$_endTime = $_startTime + 3600*24*366;
		else
			$_endTime = $_startTime + 3600*24*365;		
		
	}
	else
		die("invalid type!");
			
	$startTime = date('Y-m-d H:i:s', $_startTime);
	$endTime = date('Y-m-d H:i:s', $_endTime);		
	
	if ( file_exists($fname) && !$force )
		return true;
	
	$qry = "SELECT * FROM gamestats WHERE gamename = \"dystopia\" AND time < '".$endTime."' AND time > '".$startTime."' ORDER BY time;";
	if(DEBUG)
		echo "SQL: ".$qry."<br>";
	$res = mysql_query( $qry, $dblink );
	
	// no data, i'm sorrey!
	if (mysql_affected_rows() == 0) {
		if(DEBUG)
			echo "empty result!<br/>";
		return false;
	}
		
	$handle = fopen($fname, "w");
	
	if (DEBUG)
		echo "QRY: ".$qry."<br>";
	
	if (!$res)
		die("mysql query error: ".mysql_error());	
	
	if(DEBUG && !$handle)
		die("unable to open new stats file");

	while ( $row = mysql_fetch_array( $res, MYSQL_BOTH ) )
	{	
		$outString = $row['time'].",".$row['players']."\n";	
		
		if(!fwrite($handle, $outString))
			die("ERROR while writing");
	}
	
	fclose($handle);
	mysql_free_result($res);
	
	if(DEBUG)
		echo "done writing to '".$fname."'";
				
	return true;
}
}

?>
