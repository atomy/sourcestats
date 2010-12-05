<?php

require_once "include/config_inc.php";

function setupDB()
{
	$dblink = mysql_connect( MYSQL_HOST, MYSQL_USER, MYSQL_PW );
	
	if ( !$dblink || !mysql_select_db(MYSQL_DB) )
		die("mysql setup error: ".mysql_error());
		
	return $dblink;
}

// get unix time range for given month
function GetSecondsOfMonth( $year, $month )
{
	if( $year < 2000 && $year > 3000 )
		Throw new Exception("GetSecondsOfMonth() year out of range!");
		
	if( $month < 1 && $month > 12 )
		Throw new Exception("GetSecondsOfMonth() month out of range!");		
	
	$from = strtotime($year."-".$month."-01");
	$to = strtotime($year."-".++$month."-01");
	$diff = $to - $from;
	return $diff;
}

function IsDateValid( $day, $month, $year )
{
	return checkdate($month, $day, $year);
	
	if($day < 1 || $day > 31 || $month > 12 || $month < 1 || $year < 2000 || $year > 9999)
		return false;
	else
		return true;
}

function GetLastYearFromDB($dblink)
{
	$qry = "SELECT * FROM gamestats WHERE gamename = \"dystopia\" ORDER BY time LIMIT 1;";
	$res = mysql_query( $qry, $dblink );
	$row = mysql_fetch_array( $res, MYSQL_BOTH );
	$arrayTime = strptime($row['time'], '%Y-%m-%d %H:%M:%S');
	return ($arrayTime['tm_year'] + 1900);
}

function GetLastMonthOfYearFromDB($dblink, $year)
{
	//echo "GetLastMonthOfYear() called with year: '".$year."'<br>";
	$_fromTime = mktime(0, 0, 0, 1, 1, $year);
	$_toTime = mktime(23, 59, 59, 12, 31, $year);
	$fromTime = date('Y-m-d H:i:s', $_fromTime);
	$toTime = date('Y-m-d H:i:s', $_toTime);
	$qry = "SELECT * FROM gamestats WHERE gamename = \"dystopia\" AND time < '".$toTime."' AND time > '".$fromTime."' ORDER BY time LIMIT 1;";
	//echo "qRY: ".$qry."\n";
	$res = mysql_query( $qry, $dblink );
	$row = mysql_fetch_array( $res, MYSQL_BOTH );
	$arrayTime = strptime($row['time'], '%Y-%m-%d %H:%M:%S');
	$lastMonth = $arrayTime['tm_mon'] + 1;
	//echo "GetLastMonthOfYear() last month of year '".$year."' is '".$lastMonth."'<br>";
	return $lastMonth;
}

function GetLastDayOfMonthFromDB($dblink, $year, $month)
{
	//echo "GetLastDayOfMonth() called with year: '".$year."' and month '".$month."'<br>";
	$_fromTime = mktime(0, 0, 0, 1, 1, $year);
	$_toTime = mktime(23, 59, 59, $month, 31, $year);
	$fromTime = date('Y-m-d H:i:s', $_fromTime);
	$toTime = date('Y-m-d H:i:s', $_toTime);
	$qry = "SELECT * FROM gamestats WHERE gamename = \"dystopia\" AND time < '".$toTime."' AND time > '".$fromTime."' ORDER BY time LIMIT 1;";
	//echo "qRY: ".$qry."\n<br>";
	$res = mysql_query( $qry, $dblink );
	$row = mysql_fetch_array( $res, MYSQL_BOTH );
	$arrayTime = strptime($row['time'], '%Y-%m-%d %H:%M:%S');
	//echo "GetLastDayOfMonth() last day of month '".$month."' is '".$arrayTime['tm_mday']."'<br>";
	return ($arrayTime['tm_mday']);
}

function BuildChooseableArrays($dblink, &$days, &$months, &$years, &$types)
{
	$fromYear = GetLastYearFromDB($dblink);	
	$loopYear = $fromYear;

	while( $loopYear <= date('Y') )
	{
		if ( $loopYear > 2100 )
			die("WTF?(1)");
		$years[] = $loopYear;
		$loopYear++;
	}

	$fromMonth = GetLastMonthOfYearFromDB($dblink, $fromYear);
	$loopMonth = $fromMonth;

	while( $loopMonth <= date('n') )
	{
		if ( $loopMonth > 12 )
			die("WTF?(2)");
		$months[] = $loopMonth;
		$loopMonth++;
	}
	
	$fromDay = GetLastDayOfMonthFromDB($dblink, $fromYear, $fromMonth);
	$loopDay = $fromDay;

	while( $loopDay <= date('j') )
	{
		if ( $loopDay > 31 )
			die("WTF?(3)");
		$days[] = $loopDay;
		$loopDay++;
	}
	
	$types[TYPE_HOURLY] = "hourly";
	$types[TYPE_DAILY] = "daily";
	$types[TYPE_MONTHLY] = "monthly";	
}

function IsTypeValid($type)
{
	if ($type > TYPE_START && $type < TYPE_LAST)
		return true;
	else
		return false;
}
