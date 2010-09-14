<?php
require_once "include/config_inc.php";
require_once "include/func_inc.php";

define( DEBUG, false );

function SetupDB()
{
	$dblink = mysql_connect( MYSQL_HOST, MYSQL_USER, MYSQL_PW );
	
	if ( !$dblink || !mysql_select_db(MYSQL_DB) )
		die("mysql setup error: ".mysql_error());
		
	return $dblink;
}

function GetLastDayOfMonth($year, $month)
{
	for ($day = 28; $day < 32; $day++) 
	{
		if (!checkdate($month, $day, $year)) 
			return $day-1;
	}
}

// 2010-09-10
// %Y-%m-%d
function DayStats2File( $dblink, $year, $month, $day, $type, $force = false )
{	
	if($year == date('Y') && $month == date('m') && $day == date('j'))
		$force = true;
	
	if($type == TYPE_HOURLY)
	{
		$fname = "stats/hourly_".$year."-".$month."-".$day.".txt";
		$_startTime = mktime(0, 0, 0, $month, $day, $year);
		$_endTime = $_startTime + 3600*24;
	}
	else if ($type == TYPE_DAILY)
	{
		$fname = "stats/daily_".$year."-".$month.".txt";
		$_startTime = mktime(0, 0, 0, $month, 0, $year);
		$_endTime = $_startTime + 3600*24*GetLastDayOfMonth($year, $month);
	}
	else
		die("invalid type!");
			
	$startTime = date('Y-m-d H:i:s', $_startTime);
	$endTime = date('Y-m-d H:i:s', $_endTime);		
	
	if ( file_exists($fname) && !$force )
		return true;
	
	$qry = "SELECT * FROM gamestats WHERE gamename = \"dystopia\" AND time < '".$endTime."' AND time > '".$startTime."' ORDER BY time;";
	$res = mysql_query( $qry, $dblink );
	
	// no data, i'm sorrey!
	if (mysql_affected_rows() == 0)
		return false;
		
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

?>
