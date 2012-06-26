function millisecondsToWords(num) {
  var total_minutes = Math.floor(num / 60000);
  if (total_minutes == 0) {
    return("0min");
  }

  var minutes = total_minutes % 60;
  var total_hours = Math.floor(total_minutes / 60);
  var hours = total_hours % 24;
  var days = Math.floor(hours / 24);

  var strings = [];
  if (days > 0) {
    strings.push(days+'d');
  }
  if (hours > 0) {
    strings.push(hours+'h');
  }
  if (minutes > 0) {
    strings.push(minutes+'min');
  }

  return(strings.join(" "));
}
