<?php
    require_once "include/func_inc.php";
	require_once "include/util.php";
?>

var timeplot;

function onLoad() {

  var eventSource = new Timeplot.DefaultEventSource();

  var eventSource2 = new Timeplot.DefaultEventSource();

  var timeGeometry = new Timeplot.DefaultTimeGeometry({
    gridColor: new Timeplot.Color("#000000"),
    axisLabelsPlacement: "top"
  });

  var valueGeometry = new Timeplot.DefaultValueGeometry({
    gridColor: "#000000",
	axisLabelsPlacement: "left"
  });  

  var plotInfo = [
    Timeplot.createPlotInfo({
      id: "plot1",
      dataSource: new Timeplot.ColumnSource(eventSource,1),
      valueGeometry: valueGeometry,
      timeGeometry: timeGeometry,     
      lineColor: "#ff0000",
      fillColor: "#cc8080",
	  showValues: true
    }),
    Timeplot.createPlotInfo({
      id: "plot3",
      timeGeometry: timeGeometry,
      eventSource: eventSource2,
      lineColor: "#03212E"
	})
  ];

<?php
	if (!isset($_REQUEST['year']) || !isset($_REQUEST['month']) || !isset($_REQUEST['day']) || !isset($_REQUEST['type']) || !isset($_REQUEST['modname']))
		die("missing args");
		
  $year = floor($_REQUEST['year']);
  $month = floor($_REQUEST['month']);
  $day = floor($_REQUEST['day']);
  $type = floor($_REQUEST['type']);
  $gameName = $_REQUEST['modname'];
  
  if (!IsTypeValid($type))
	die("invalid type");
  
  if (!IsDateValid($day, $month, $year))
	die("invalid date");
	
	if(!Util::isModNameValid($gameName))
		die("invalid gameName");
	
  if ( $type == TYPE_HOURLY )
	$fname = "stats/".$gameName."hourly_".$year."-".$month."-".$day.".txt";
  else if ($type == TYPE_DAILY )
	$fname = "stats/".$gameName."daily_".$year."-".$month.".txt";
  else if ($type == TYPE_MONTHLY )
	$fname = "stats/".$gameName."monthly_".$year."-".$month.".txt";	
  else
    die("unknown type!");
?>

  timeplot = Timeplot.create(document.getElementById("plotter"), plotInfo);
  timeplot.loadText("<?php echo $fname;?>", ",", eventSource);
}

var resizeTimerID = null;

function onResize() {
    if (resizeTimerID == null) {
        resizeTimerID = window.setTimeout(function() {
            resizeTimerID = null;
            timeplot.repaint();
        }, 100);
    }
}