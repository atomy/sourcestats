<?php

class DatedRequest {
    private $day;
    private $month;
    private $year;
    private $type;
    private $modname;
    
    public function __construct($day, $month, $year, $type, $modname) {
        $this->day = $day;
        $this->month = $month;
        $this->year = $year;
        $this->type = $type;
        $this->modname = $modname;
    }
    
    public function getDay() {
        return $this->day;
    }
    
    public function getMonth() {
        return $this->month;
    }
    
    public function getYear() {
        return $this->year;
    }
    
    public function getModName() {
        return $this->modname;
    }
    
    public function getType() {
        return $this->type;
    }
}