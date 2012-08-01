function minutesToWords(total_minutes) {
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

function millisecondsToWords(num) {
  return minutesToWords(Math.floor(num / 60000));
}

function diffMinutesToWords(first, second) {
  return minutesToWords(first - second);
}

function dateToMinutes(date) {
  return date.getTime() / 60000;
}

function dateToYMD(d) {
  var month = d.getMonth() + 1;
  var day = d.getDate();
  return "" + (month < 10 ? "0" + month : month) + "/" + (day < 10 ? "0" + day : day) + "/" + d.getFullYear();
}

function dateToHM(d) {
  var hours = d.getHours();
  var minutes = d.getMinutes();
  return "" + (hours < 10 ? "0" + hours : hours) + ":" + (minutes < 10 ? "0" + minutes : minutes);
}

function isSameDay(first, second) {
  return first.getUTCDate() == second.getUTCDate() &&
    first.getUTCMonth() == second.getUTCMonth() &&
    first.getUTCFullYear() == second.getUTCFullYear();
}
