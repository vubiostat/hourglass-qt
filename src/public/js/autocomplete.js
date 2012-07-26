var activities, tags, projects;

function getActivities() {
  $.get('/activities', function(data) {
    var d = new Dictionary();
    $.each(data, function(i, object) {
      d.add(object);
    });
    activities = d;
  }, 'json');
}

function getTags() {
  $.get('/tags', function(data) {
    var d = new Dictionary();
    $.each(data, function(i, name) {
      d.add(name);
    });
    tags = d;
  }, 'json');
}

function splitTags(val) {
  return val.split(/,\s*/);
}

function extractLastTag(term) {
  return splitTags(term).pop();
}

function getProjects() {
  $.get('/projects', function(data) {
    var d = new Dictionary();
    $.each(data, function(i, name) {
      d.add(name);
    });
    projects = d;
  }, 'json');
}

$(function() {
  // these return immediately
  getActivities();
  getTags();
  getProjects();

  $('input.activity-name').autocomplete({
    minLength: 0,
    delay: 0,
    source: function(request, response) {
      var result = [];
      var parts = request.term.split('@');
      if (parts.length == 2) {
        if (projects != null) {
          var values = projects.match(parts[1]);
          if (values != null) {
            result = values;
          }
        }
      }
      else {
        if (activities != null) {
          var values = activities.match(request.term);
          if (values != null) {
            result = values;
          }
        }
      }
      response(result);
    },
    focus: function(event, ui) {
      var item = ui.item;
      var itemParts = item.value.split('@');
      var obj = $(event.target);
      var objParts = obj.val().split('@');

      if (objParts.length == 2 && itemParts.length == 1) {
        /* autocomplete only the project */
        obj.val(objParts[0] + '@' + item.value);
      }
      else {
        obj.val(item.value);
      }

      return false;
    },
    select: function(event, ui) {
      return false;
    }
  });

  $('#select-activity').click(function(e) {
    e.preventDefault();
    $('input.activity-name').focus().autocomplete('search', '');
  });

  $('input.activity-tags')
    // don't navigate away from the field on tab when selecting an item
    .bind("keydown", function(event) {
      if (event.keyCode === $.ui.keyCode.TAB && $(this).data("autocomplete").menu.active) {
        event.preventDefault();
      }
    })
    .autocomplete({
      minLength: 0,
      delay: 0,
      source: function(request, response) {
        var result = [];
        var term = extractLastTag(request.term);
        if (tags != null) {
          var values = tags.match(term);
          if (values != null) {
            result = values;
          }
        }
        response(result);
      },
      focus: function() {
        // prevent value inserted on focus
        return false;
      },
      select: function(event, ui) {
        var terms = splitTags(this.value);

        // remove the current input
        terms.pop();

        // add the selected item
        terms.push(ui.item.value);

        // add placeholder to get the comma-and-space at the end
        terms.push("");

        this.value = terms.join(", ");
        return false;
      }
    });

  $('.activity-started-at-date, .activity-ended-at-date').datepicker();
  $('.activity-started-at-time, .activity-ended-at-time').timepicker({});
});
