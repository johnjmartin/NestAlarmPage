var app = angular.module('alarmApp', []);

app.controller('alarmController', function($scope){
	$scope.alarms = [];
	$scope.days = {sunday:false,
					monday: false,
					tuesday: false,
					wednesday: false,
					thursday: false,
					friday: false,
					saturday: false};

	var tDate = new Date("2015-03-25T12:30:00");
	var alarm1 = {start_time: tDate, days: ["Monday"]};
	tDate = new Date("2015-03-25T11:30:00");
	var alarm2 = {start_time: tDate, days: ["Tuesday"]};
	tDate = new Date ("2015-03-25T10:30:00");
	var alarm3 = {start_time: tDate, days: ["Thursday"]};
	tDate = new Date ("2015-03-25T13:30:00");
	var alarm4 = {start_time: tDate, days: ["Wednesday","Friday"]};
	tDate = new Date ("2015-03-25T14:30:00");
	var alarm5 = {start_time: tDate, days: ["Saturday", "Sunday"]};
	$scope.alarms.push(alarm1, alarm2, alarm3, alarm4, alarm5);
	$scope.newAlarm = {start_time: '', days: []};

	$scope.create = function(){
		if ($scope.days.sunday) $scope.newAlarm.days.push("Sunday"); console.log("Sunday");
		if ($scope.days.monday) $scope.newAlarm.days.push("Monday"); console.log("Monday");
		if ($scope.days.tuesday) $scope.newAlarm.days.push("Tuesday"); console.log*("Tuesday");
		if ($scope.days.wednesday) $scope.newAlarm.days.push("Wednesday"); console.log("Wednesday");
		if ($scope.days.thursday) $scope.newAlarm.days.push("Thursday"); console.log("Thursday");
		if ($scope.days.friday) $scope.newAlarm.days.push("Friday"); console.log("Friday");
		if ($scope.days.saturday) $scope.newAlarm.days.push("Saturday"); console.log("Saturday");

		if ($scope.newAlarm.start_time.getHours() == 0) $scope.newAlarm.start_time.setHours(12);

		if ($scope.newAlarm.start_time.getHours() > 12){
			$scope.newAlarm.start_time.setHours(24 - $scope.newAlarm.start_time.getHours());
			$scope.newAlarm.AMorPM = 'PM';
		}


		$scope.alarms.push($scope.newAlarm);
		console.log("Success!" + $scope.newAlarm.start_time.getHours() + ":" + $scope.newAlarm.start_time.getMinutes());
		$scope.newAlarm = {start_time: '', days: []};
	};
});