const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <title>OldSkoolCoder Control Centre</title>
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css">
<body>
    <div class="container">
        <div class="row">
            <div class="col-4"></div>
            <div class="col-8">
                <form action="/" method="POST">
                    <div class="form-group">
                        <label for="user">Enter Username:</label>
                        <input id="user" type="text" placeholder="Username Here..." maxlength="50" class="form-control">
                    </div>
                    <div class="form-group">
                        <label for="points">Enter Points:</label>
                        <input id="points" type="text" placeholder="Points Here..." maxlength="10" class="form-control">
                    </div>
                    <div class="form-group">
                        <label for="bet">Enter Bet:</label>
                        <input id="bet" type="text" placeholder="Bet Here..." maxlength="10" class="form-control" >
                    </div>
                    <button class="btn btn-primary" onclick="myFunctionSlots()">Send Slots Info</button>
                </form>
            </div>
        </div> 
        <div class="row">
            <div class="col-4"></div>
            <div class="co-8">
                <form action="/" method="POST">
                <div class="form-group">
                    <label for="clock">Choose a clock:</label>

                    <select class="form-control" name="clock" id="clocks">
                    <option value="0">Analog</option>
                    <option value="2">Tetris</option>
                    <option value="1">Pong</option>
                    </select>
                    <button class="btn btn-success" onclick="myFunctionClock()">Change Clock</button>
                </div>
                </form>
            </div>
        </div> 
    </div>
</body>
<script>
function myFunctionClock() {
  var n = document.getElementById("clocks");
  var request = new XMLHttpRequest();
  var strLine = "myClock?ClockMode=" + n.value;
  request.open("GET", strLine, false);
  request.send(null);
}

function myFunctionSlots() {
  var userName = document.getElementById("user");
  var usersPoints = document.getElementById("points");
  var usersBet = document.getElementById("bet");
  var request = new XMLHttpRequest();
  var strLine = "mySlots?userName=" + userName.value + "&userPoints=" + usersPoints.value + "&userBet=" + usersBet.value;
  request.open("GET", strLine, false);
  request.send(null);
}
</script>
</html>
)=====";