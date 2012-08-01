function dayStart() {
  return new Date($(".activity-started-at-date").val() + " " + dayStartTime);
}
function dayStartMinutes() {
  return dateToMinutes(dayStart());
}
function dayEnd() {
  return new Date($(".activity-started-at-date").val() + " " + dayEndTime);
}
function dayEndMinutes() {
  return dateToMinutes(dayEnd());
}
function startedAt() {
  return new Date($(".activity-started-at-date").val() + " " + $(".activity-started-at-time").val());
}
function startedAtMinutes() {
  return dateToMinutes(startedAt());
}
function setStartedAt(minutes) {
  var d = new Date(minutes * 60000);
  $('.activity-started-at-date').val(dateToYMD(d));
  $('.activity-started-at-time').val(dateToHM(d));
}
function endedAt() {
  return new Date($(".activity-ended-at-date").val() + " " + $(".activity-ended-at-time").val());
}
function endedAtMinutes() {
  return dateToMinutes(endedAt());
}
function setEndedAt(minutes) {
  var d = new Date(minutes * 60000);
  $('.activity-ended-at-date').val(dateToYMD(d));
  $('.activity-ended-at-time').val(dateToHM(d));
}
function updateTimedDuration() {
  var duration = diffMinutesToWords(endedAtMinutes(), startedAtMinutes());
  $("#timed-slider .duration").html(duration == "" ? "&nbsp;" : duration);
}
function updateTimedDurationSlider() {
  var demin = dayEndMinutes();
  var dsmin = dayStartMinutes();
  var emin = endedAtMinutes();
  var smin = startedAtMinutes();
  if (isSameDay(startedAt(), endedAt()) && demin > dsmin &&
  emin >= smin && emin <= demin && emin >= dsmin &&
  smin >= dsmin && smin <= demin) {
    timedDurationSlider.slider('enable');
    timedDurationSlider.slider('values', 0, startedAtMinutes());
    timedDurationSlider.slider('values', 1, endedAtMinutes());
  }
  else {
    timedDurationSlider.slider('disable');
  }
}
function updateUntimedDuration() {
  var hours = parseInt($('input.activity-duration-hours').val());
  var minutes = parseInt($('input.activity-duration-minutes').val());
  var totalMinutes = hours * 60 + minutes;
  if (minutes > 59 || minutes < 0) {
    hours = Math.floor(totalMinutes / 60);
    minutes = totalMinutes % 60;
    $('input.activity-duration-hours').val(hours);
    $('input.activity-duration-minutes').val(minutes);
  }

  $('input.activity-duration').val(totalMinutes * 60);
  if (totalMinutes < 1 || totalMinutes > 480) {
    untimedDurationSlider.slider('disable');
  }
  else {
    untimedDurationSlider.slider('enable');
    untimedDurationSlider.slider('option', 'value', totalMinutes);
  }
}
var incrementInterval, incrementTimeout;
function startIncrementing(obj, by) {
  var repeat, delay;
  if (obj.hasClass('activity-duration-hours')) {
    repeat = 200;
    delay = 0;
  }
  else {
    repeat = 50;
    delay = 200;
  }
  increment(obj, by);

  var callback = function() {
    incrementInterval = setInterval(function() {
      increment(obj, by);
    }, repeat);
  };
  if (delay > 0) {
    incrementTimeout = setTimeout(callback, delay);
  }
  else {
    callback();
  }
}
function increment(obj, by) {
  var val = parseInt(obj.val());
  var max = obj.hasClass('activity-duration-hours') ? 24 : 99;
  var min = obj.hasClass('activity-duration-hours') ? 0 : -9;
  if (by > 0) {
    if (val < max) {
      obj.val(val + by);
      obj.trigger('change');
    }
  }
  else if (by < 0) {
    if (val > min) {
      obj.val(val + by);
      obj.trigger('change');
    }
  }
}

$(function() {
  var timedForm = $('#timed-tab form');
  var untimedForm = $('#untimed-tab form');
  var currentForm = untimed ? untimedForm : timedForm;

  timedForm.add(untimedForm).find('input[type=text]').keydown(function(e) {
    if (e.which == $.ui.keyCode.ENTER) {
      $(this).closest('form').submit();
    }
  });

  var resizeTimer;
  $('#tabs').tabs({
    select: function(event, ui) {
      switch (ui.index) {
        case 0:
          currentForm = timedForm;
          break;
        case 1:
          currentForm = untimedForm;
          break;
      }
    },
    show: function(event, ui) {
      if (typeof(qt) == 'object') {
        qt.resizeViewHeight();
      }
    },
    selected: untimed ? 1 : 0
  });

  $('#save-button, #cancel-button').button();
  $('#save-button').click(function(e) {
    currentForm.submit();
  });
  $('#cancel-button').click(function(e) {
    e.preventDefault();
    window.close();
  });

  var checkbox = $('#in-progress-checkbox');
  checkbox.click(function(e) {
    if (checkbox.is(':checked')) {
      timedForm.find('.activity-ended-at-time').attr('disabled', true);
      timedForm.find('.activity-ended-at-date').attr('disabled', true);
      timedDurationSlider.slider('disable');
    }
    else {
      timedForm.find('.activity-ended-at-time').attr('disabled', false);
      timedForm.find('.activity-ended-at-date').attr('disabled', false);
      timedDurationSlider.slider('enable');
    }
  });

  $('form').submit(function(e) {
    e.preventDefault();
    $.post(submitUrl, $(this).serialize(), function(data, status) {
      if (data && !data.errors) {
        window.opener.updateUI(data);
        window.close();
      }
      else {
        // TODO: show reason for failure
      }
    }, 'json');
  });

  timedDurationSlider = $('#timed-slider .slider').slider({
    range: true,
    min: dayStartMinutes(),
    max: dayEndMinutes(),
    values: [0, 0],
    slide: function(event, ui) {
      setStartedAt(ui.values[0]);
      setEndedAt(ui.values[1]);
      updateTimedDuration();
    }
  });
  updateTimedDurationSlider();

  $('input.activity-started-at, input.activity-ended-at').change(function() {
    updateTimedDuration();
    updateTimedDurationSlider();
  });
  updateTimedDuration();

  untimedDurationSlider = $('#untimed-slider').slider({
    range: "min",
    min: 1,
    max: 480,
    slide: function(event, ui) {
      $('input.activity-duration').val(ui.value * 60);

      $('input.activity-duration-hours').val(Math.floor(ui.value / 60));
      $('input.activity-duration-minutes').val(ui.value % 60);
    }
  });
  updateUntimedDuration();

  $('input.activity-duration-hours, input.activity-duration-minutes').
    change(updateUntimedDuration).
    keydown(function(e) {
      var obj = $(this);
      switch (e.which) {
        case $.ui.keyCode.UP:
          increment(obj, 1);
          break;
        case $.ui.keyCode.DOWN:
          increment(obj, -1);
          break;
      }
    });

  $('td.untimed-duration .hours .up').mousedown(function(e) {
    if (e.which == 1) {
      var obj = $('input.activity-duration-hours');
      startIncrementing(obj, 1);
    }
  });
  $('td.untimed-duration .hours .down').mousedown(function(e) {
    if (e.which == 1) {
      var obj = $('input.activity-duration-hours');
      startIncrementing(obj, -1);
    }
  });
  $('td.untimed-duration .minutes .up').mousedown(function(e) {
    if (e.which == 1) {
      var obj = $('input.activity-duration-minutes');
      startIncrementing(obj, 1);
    }
  });
  $('td.untimed-duration .minutes .down').mousedown(function(e) {
    if (e.which == 1) {
      var obj = $('input.activity-duration-minutes');
      startIncrementing(obj, -1);
    }
  });
  $('td.untimed-duration').find('.up, .down').bind('mouseup mouseleave', function(e) {
    if (e.which == 1) {
      clearTimeout(incrementTimeout);
      clearInterval(incrementInterval);
    }
  });
});
