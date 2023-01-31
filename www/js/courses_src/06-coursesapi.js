/*
* Author: ikki Chung
* Date: 9 Apr 2019
* Description:
*      SMIMS VILS RTLS Solutions
*/

var glMapAreas = [];
// var glLocators = [];
// var gl_locator_num = 0;
// var gl_locator_load_index = 0;

var LOAD_DEVICE_NUM = 20;
var PreLoadIndex = -1;
var CurrentLoadIndex = 0;


// start loading
check_loading_status();

function check_loading_status()
{
    //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" CurrentLoadIndex:" + CurrentLoadIndex);
    switch(CurrentLoadIndex)
    {
        case 0:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                load_mapareas();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                var dropdown_UI = document.getElementById('area_list_dropdownUI');
                if ( dropdown_UI != undefined )
                {
                    load_Items_to_dropdown_UI("btn-success", dropdown_UI, glMapAreas);
                }

                load_event_exist();
            }
        break;
    }

    if ( CurrentLoadIndex != 2 )
        setTimeout(check_loading_status, 100);
    else
    {
        console.log("check_loading_status() Stop.");
    }
}

function load_mapareas()
{
    var targetURL = gl_target_server+ "/php/map.php";

    glMapAreas = [];

    $.getJSON(targetURL, 'project_id='+project_id, function(data)
    {
        var count = 0;
        $.each(data, function(itemindex, area)
        {
            var geoJSON = JSON.parse(area);

            glMapAreas[count] = {};

            if(geoJSON.properties.areatype == 4)
            {
                glMapAreas[count].itemid = geoJSON.properties.areaid;
                glMapAreas[count].itemname = geoJSON.properties.areaname;

                //console.log("load_mapareas() geoJSON:"+geoJSON);
                // console.log("load_mapareas() areaid:"+geoJSON.properties.areaid+", areaname:"+geoJSON.properties.areaname);

                count++;
            }
        });
    })
    .success(function() {
        CurrentLoadIndex = 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_event_exist()
{
    var targetURL = gl_target_server + "/php/course.php";
    var p = $.getJSON(targetURL, 'loadcourses=1'+'&project_id='+project_id, function(data)
    {
        $.each(data, function(courseId, value)
        {
            //var courseId = value['courseId'];
            var resourceId = value['resourceId'];
            var title = value['title'];
            var description = value['description'];
            var startDate = value['start'];
            var endDate = value['end'];
            var startTime = value['startTime'];
            var endTime = value['endTime'];
            var startRecur = value['startRecur'];
            var endRecur = value['endRecur'];
            var backgroundColor = value['backgroundColor'];
            var borderColor = value['borderColor'];

            var allDay = false;
            if (value['allDay'] === '1')
                allDay = true;

            //console.log("courseId:"+courseId+" resourceId:"+resourceId+" title:"+title);


            if (startRecur.length > 0 && endRecur.length > 0)
            {
                glCalendar.addEvent({
                    resourceId      : resourceId,
                    id              : courseId,
                    title           : title,
                    description     : description,
                    allDay          : allDay,
                    startTime       : startTime,
                    endTime         : endTime,
                    startRecur      : startRecur,
                    endRecur        : endRecur,
                    backgroundColor : backgroundColor,
                    borderColor     : borderColor
                  });
            }
            else
            {
                glCalendar.addEvent({
                    resourceId      : resourceId,
                    id              : courseId,
                    title           : title,
                    description     : description,
                    allDay          : allDay,
                    start           : startDate,
                    end             : endDate,
                    backgroundColor : backgroundColor,
                    borderColor     : borderColor
                  });
            }
        });
    })
    .success(function() {
        CurrentLoadIndex = 2;
    })
    .error(function() {
        CurrentLoadIndex = 2;
    })
    .complete(function() {
        CurrentLoadIndex = 2;
    });
    //setTimeout(function(){ p.abort(); }, 5000);
}

function load_Items_to_dropdown_UI(uitype, item_selection_btn_group, items)
{
    var itemid = "";
    var itemname = "";

    if ( items.length > 0 )
    {
        itemid = items[0].itemid;
        itemname = items[0].itemname;
    }

    var btn_group1_button1 = document.createElement('button');
    item_selection_btn_group.appendChild(btn_group1_button1);
    btn_group1_button1.setAttribute("class","btn "+uitype);
    btn_group1_button1.setAttribute("data-id", itemid);
    btn_group1_button1.id = "item_show";
    var btn_group1_button1_text = document.createTextNode(itemname);
    btn_group1_button1.appendChild(btn_group1_button1_text);

    // dropdown button
    var btn_group1_button2 = document.createElement('button');
    item_selection_btn_group.appendChild(btn_group1_button2);
    btn_group1_button2.setAttribute("class","btn "+uitype+" dropdown-toggle");
    btn_group1_button2.setAttribute("data-toggle", "dropdown");
    btn_group1_button2.id = "item_id";

    var btn_group1_button2_span1 = document.createElement('span');
    btn_group1_button2.appendChild(btn_group1_button2_span1);
    btn_group1_button2_span1.setAttribute("class","caret");

    var btn_group1_button2_span2 = document.createElement('span');
    btn_group1_button2.appendChild(btn_group1_button2_span2);
    btn_group1_button2_span2.setAttribute("class","sr-only");
    //var btn_group1_button2_span2_text = document.createTextNode("Toggle Dropdown");
    //btn_group1_button2_span2.appendChild(btn_group1_button2_span2_text);

    var dropdown_ul = document.createElement('ul');
    item_selection_btn_group.appendChild(dropdown_ul);
    dropdown_ul.setAttribute("class", "dropdown-menu");
    dropdown_ul.setAttribute("role", "menu");
    //dropdown_ul.id = "dropdown_menu";

    for (var i = 0; i < items.length; i++)
    {
        var dropdown_li_0 = document.createElement('li');
        dropdown_ul.appendChild(dropdown_li_0);

        var dropdown_li_0_href = document.createElement('a');
        dropdown_li_0.appendChild(dropdown_li_0_href);
        dropdown_li_0_href.setAttribute("href", "javascript:;");
        dropdown_li_0_href.setAttribute("onclick", "item_select("+ i +");return false;");
        dropdown_li_0_href.setAttribute("data-id", items[i].itemid);
        dropdown_li_0_href.id = "select_item_"+i;

        //console.log("load_Items_to_dropdown_UI() items[i].itemname:"+items[i].itemname);
        //if (items[i].type == "locator")
        //{
            var span_anchor = document.createElement("span");
            span_anchor.textContent = items[i].itemname;
            span_anchor.setAttribute("class","badge bg-yellow-active");
            dropdown_li_0_href.appendChild(span_anchor);
        //}
    }
}

function item_select(itemidx)
{
    console.log("item_select() itemidx:"+itemidx);

    var select_value = $("#select_item_"+itemidx).html();
    var select_id = $("#select_item_"+itemidx).attr("data-id");

    $("#item_show").html(select_value);
    $("#item_show").attr("data-id", select_id);
}

function create_new_course(project_id, resourceId, courseId, title, description, allDay, startDate, endDate, startTime, endTime, startRecur, endRecur, backgroundColor, borderColor)
{

    console.log("create_new_event() resourceId:"+resourceId + " courseId:"+courseId);
    console.log("create_new_event() title:"+title+" description:"+description);
    console.log("create_new_event() currColor:"+currColor+" allDay:"+allDay);
    console.log("create_new_event() startTime:"+startTime+" endTime:"+endTime);
    console.log("create_new_event() startDate:"+startDate+" endDate:"+endDate);
    console.log("create_new_event() startRecur:"+startRecur+" endRecur:"+endRecur);


    var json = {};
    json['project_id'] = ''+project_id;
    json['CREATE_COURSE'] = 1;
    json['resourceId'] = resourceId;
    json['courseId'] = courseId;
    json['title'] = title;
    json['description'] = description;
    if (allDay)
        json['allDay'] = 1;
    else
        json['allDay'] = 0;
    json['start'] = startDate;
    json['end'] = endDate;
    json['url'] = '';
    json['startTime'] = startTime;
    json['endTime'] = endTime;
    json['daysOfWeek'] = '';
    json['startRecur'] = startRecur;
    json['endRecur'] = endRecur;
    json['backgroundColor'] = backgroundColor;
    json['borderColor'] = borderColor;

    return jQuery.ajax({
    url: '/php/course.php',
    type: 'POST',
    contentType: "application/json",
    dataType: "json",
    data: JSON.stringify(json),
    success:
        function(response)
        {
            console.log(response);
        },
    error:
        function(xhr, status, error)
        {
           console.log(error);
           console.log(xhr.responseText);
        }
    });
}

function fetch_rollcall_onview(div_RollCall, showDate, courseId)
{
    // while (div_RollCall.firstChild)
    // {
    //     div_RollCall.removeChild(div_RollCall.firstChild);
    // }

    var targetURL = gl_target_server + "/php/course.php";
    $.getJSON(targetURL, 'loadcourserollcallrecord=1'+'&project_id='+project_id+'&courseId='+courseId+'&infoOfDate='+showDate, function(data)
    {
        var tagIds = data['tags'];
        var tags = data['tagname'];

        var div_Number = document.createElement("div");
        var span_Number = document.createElement("span");
        span_Number.setAttribute("class","bg-blue-active");
        //span_Number.textContent = '出席人數:' + tags.length;
        var H4_Number = document.createElement("H4");
        var p_Number = document.createElement("p");
        H4_Number.appendChild(p_Number);
        span_Number.appendChild(H4_Number);
        div_Number.appendChild(span_Number);
        div_RollCall.appendChild(div_Number);

        if (tags == undefined || tags.length == 0)
        {
            p_Number.setAttribute("class","text-red");
            p_Number.textContent = '無人出席';

            // var span_tag = document.createElement("span");
            // span_tag.setAttribute("class","bg-red-active");
            // span_tag.textContent = '無人出席';
            // div_RollCall.appendChild(span_tag);
            return;
        }

        p_Number.setAttribute("class","text-light-blue");
        p_Number.textContent = '出席人數:' + tags.length;


        // console.log('courseId:'+ data['courseId']);
        // console.log('infoOfDate:'+ data['infoOfDate']);
        // console.log('tags:'+ data['tags']);




        var div_Tags = document.createElement("div");
        div_RollCall.appendChild(div_Tags);

        for(var index=0;index<tags.length;index++)
        {
            var span_tag = document.createElement("span");
            span_tag.setAttribute("class","badge bg-green");
            span_tag.textContent = tags[index];
            if ( tags[index].length == 0 )
                span_tag.textContent = tagIds[index];
            div_Tags.appendChild(span_tag);
        }

    })
    .success(function() {

    })
    .error(function() {

    })
    .complete(function() {

    });
}

function deletecourse_click(project_id, todayDate, title, courseId)
{
    if (confirm('Are you sure you want to delete this course '+title+' on '+todayDate+' ?'))
    {
        // delete it!
        delete_course(project_id, todayDate, courseId);
    }
    else
    {
        // Do nothing!
    }
}

function delete_course(project_id, todayDate, courseId)
{

    console.log("delete_course() todayDate:"+todayDate+", courseId:"+courseId);

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['DELETE_COURSE'] = 1;
    jsondata['todayDate'] = todayDate;
    jsondata['courseId'] = courseId;

    var targetURL = gl_target_server + "/php/course.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      contentType: "application/json",
      dataType: "json",
      data: JSON.stringify(jsondata),
      success:
        function(response)
        {
            if ( response.status == 400 )
            {
                console.log("DELETE Error! response = " + JSON.stringify(response));
            }
            else
            {
                var event = glCalendar.getEventById(courseId);
                //console.log("event:" + event);
                event.remove();

                glCalendar.rerenderEvents();
            }
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });
   return false;
}
