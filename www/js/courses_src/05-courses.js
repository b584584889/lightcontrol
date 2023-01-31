/*
 * Author: ikki Chung
 * Date: 27 Mar 2019
 * Description:
 *      SMIMS VILS RTLS Solutions
 *      https://fullcalendar.io/
 **/


var glCalendar;

//init_events($('#external-events div.external-event'))

//Date range picker with time picker
// $('#reservationtime').daterangepicker({ timePicker: true, timePickerIncrement: 30, format: 'MM/DD/YYYY h:mm A' })
//Date range picker
$('#DateRangeInputText').daterangepicker({locale:{format: 'YYYY/MM/DD'}})

//Timepicker
$('.timepicker').timepicker({
  showInputs: false,
  showMeridian: false
})

//Date picker
$('#DatePickerInputText').datepicker({
  autoclose: true,
  format: "yyyy-mm-dd"
});
$('#DatePickerInputText').datepicker("setDate", new Date());

function init_events(ele)
{

    ele.each(function ()
    {
        // create an Event Object (http://arshaw.com/fullcalendar/docs/event_data/Event_Object/)
        // it doesn't need to have a start or end
        var eventObject =
        {
            id   : $(this).attr('id'),
            title: $.trim($(this).text()) // use the element's text as the event title
        };

        // store the Event Object in the DOM element so we can get to it later
        $(this).data('eventObject', eventObject);

        // make the event draggable using jQuery UI
        $(this).draggable({
            zIndex        : 1070,
            revert        : true, // will cause the event to go back to its
            revertDuration: 0  //  original position after the drag
        });
    });
}

document.addEventListener('DOMContentLoaded', function()
{
    var Calendar = FullCalendar.Calendar;
    // var Draggable = FullCalendarInteraction.Draggable;

    //var containerEl = document.getElementById('external-events');
    var calendarEl = document.getElementById('calendar');
    //var checkbox = document.getElementById('drop-remove');

    // -----------------------------------------------------------------
    // initialize the external events
    // -----------------------------------------------------------------
    // new Draggable(containerEl,
    // {
    //     itemSelector: '.external-event',
    //     eventData: function(eventEl)
    //     {
    //         return {
    //             id  :9999,
    //             title: eventEl.innerText
    //         };
    //     }
    // });

    // -----------------------------------------------------------------
    // initialize the calendar
    // -----------------------------------------------------------------
    //Date for the calendar events (dummy data)
    var date = new Date();
    var d    = date.getDate();
    var m    = date.getMonth();
    var y    = date.getFullYear();
    glCalendar = new Calendar(calendarEl,
    {
        locale: 'zh-tw',
        //timeZone: 'UTC',
        timeZone: 'local', // the default (unnecessary to specify)
        plugins: [ 'dayGrid', 'timeGrid', 'list', 'interaction'],
        defaultView: 'dayGridMonth',
        header:
        {
            left: 'prev,next today',
            center: 'title',
            right: 'dayGridMonth,timeGridWeek,timeGridDay,listWeek'
        },
        buttonText: {
            today: '今天',
            month: '月',
            week : '週',
            day  : '天',
            list : '列表'
        },
        // resources: [
        //     { id: 'a', title: 'Room A' },
        //     { id: 'b', title: 'Room B' },
        //     { id: 'c', title: 'Room C' },
        //     { id: 'd', title: 'Room D' }
        // ],
        navLinks: true, // can click day/week names to navigate views
        editable: false,
        droppable : true, // this allows things to be dropped onto the calendar !!!
        eventLimit: true, // allow "more" link when too many events
        // events: '/demo-events.json?overload-day' // TODO: use BASE_URL somehow
        events    : [
            // {
            // resourceId     : 'a',
            // id             : '1001',
            // title          : 'All Day Event',
            // description    : 'description for All Day Event',
            // start          : new Date(y, m, 1),
            // allDay         : true,
            // backgroundColor: '#f56954', //red
            // borderColor    : '#f56954' //red
            // },
            // {
            // resourceId     : 'a',
            // id             : '1002',
            // title          : 'Long Event',
            // description    : 'description for Long Event',
            // start          : new Date(y, m, d + 2, 10, 30),
            // end            : new Date(y, m, d + 3, 14, 45),
            // allDay         : false,
            // backgroundColor: '#f39c12', //yellow
            // borderColor    : '#f39c12' //yellow
            // },
            // {
            // resourceId     : 'b',
            // id             : '1003',
            // title          : 'Meeting',
            // description    : 'description for Meeting Event',
            // start          : new Date(y, m, d, 14, 30),
            // end            : new Date(y, m, d, 15, 30),
            // allDay         : false,
            // backgroundColor: '#0073b7', //Blue
            // borderColor    : '#0073b7' //Blue
            // },
            // {
            // resourceId     : 'c',
            // id             : '1004',
            // title          : 'Lunch',
            // description    : 'description for Lunch Event',
            // start          : new Date(y, m, d, 12, 0),
            // end            : new Date(y, m, d, 14, 0),
            // allDay         : false,
            // backgroundColor: '#00c0ef', //Info (aqua)
            // borderColor    : '#00c0ef' //Info (aqua)
            // },
            // {
            // resourceId     : 'd',
            // id             : '1005',
            // title          : 'Birthday Party',
            // description    : 'description for Birthday Party Event',
            // start          : new Date(y, m, d + 1, 19, 0),
            // end            : new Date(y, m, d + 1, 22, 30),
            // allDay         : false,
            // backgroundColor: '#00a65a', //Success (green)
            // borderColor    : '#00a65a' //Success (green)
            // },
            // {
            // resourceId     : 'd',
            // id             : '1006',
            // title          : 'Click for Google',
            // description    : 'description for Click for Google Event',
            // start          : new Date(y, m, 28),
            // end            : new Date(y, m, 29),
            // url            : 'http://google.com/',
            // backgroundColor: '#3c8dbc', //Primary (light-blue)
            // borderColor    : '#3c8dbc' //Primary (light-blue)
            // },
            // {
            // resourceId     : '4',
            // id             : '1007',
            // title          : 'Every Week',
            // description    : 'description for Every Week Event',
            // startTime      : '06:00',
            // endTime        : '07:00',
            // daysOfWeek     : [1, 2, 4], // 0:sunday
            // startRecur     : new Date(y, m, 1),
            // endRecur       : new Date(y, m, 30),
            // backgroundColor: '#3c8dbc', //Primary (light-blue)
            // borderColor    : '#3c8dbc' //Primary (light-blue)
            // }
        ],
        drop: function(info)
        {
            console.log("fullCalendar::drop()");
            console.log("date:"+moment(info.date).utc().format("YYYY/MM/DD HH:mm:ss"));
            console.log("allDay:"+info.allDay);

            var originalEventObject = $(info.draggedEl).data('eventObject')

            // retrieve the dropped element's stored Event Object
            //var originalEventObject = $(this).data('eventObject')

            console.log("fullCalendar::drop() originalEventObject.title:" + originalEventObject.title);
            console.log("fullCalendar::drop() originalEventObject.id:" + originalEventObject.id);

            // is the "remove after drop" checkbox checked?
            // if (checkbox.checked)
            // {
            //     // if so, remove the element from the "Draggable Events" list
            //     info.draggedEl.parentNode.removeChild(info.draggedEl);
            // }
        },
        eventDrop : function (info)
        {
            console.log("fullCalendar::eventDrop() info.event.id:" + info.event.id + ", title:"+ info.event.title + ", allDay:"+ info.event.allDay);
            console.log("fullCalendar::eventDrop() info.event.start:"+moment(info.event.start).utc().format("YYYY/MM/DD HH:mm:ss")+", info.event.end:"+moment(info.event.end).utc().format("YYYY/MM/DD HH:mm:ss"));
            //eventDropInfo.revert();
        },
        eventResize : function (info)
        {
            console.log("fullCalendar::eventResize() info.event.id:" + info.event.id + ", title:"+ info.event.title);
            console.log("fullCalendar::eventResize() info.event.start:"+moment(info.event.start).utc().format("YYYY/MM/DD HH:mm:ss")+", info.event.end:"+moment(info.event.end).utc().format("YYYY/MM/DD HH:mm:ss"));
            //info.revert();
        },
        eventRender: function(info)
        {
            var view = glCalendar.view;
            // console.log("fullCalendar::eventRender() view:" + view);
            // console.log("fullCalendar::eventRender() view.type:" + view.type);
            // console.log("fullCalendar::eventRender() view.currentStart:" + view.currentStart + ", view.currentEnd:"+ view.currentEnd);
            // console.log("fullCalendar::eventRender() view.activeStart:" + view.activeStart + ", view.activeEnd:"+ view.activeEnd);

            // dayGridMonth, timeGridWeek, timeGridDay
            if (view.type === 'timeGridDay')
            {
                // console.log("fullCalendar::eventRender() info.event.id:" + info.event.id + ", title:"+ info.event.title + ", description:"+info.event.extendedProps.description);
                // console.log("fullCalendar::eventRender() info.event.extendedProps.resourceId:" + info.event.extendedProps.resourceId );
                // console.log("fullCalendar::eventRender() info.isStart:" + info.isStart + ", isEnd:"+ info.isEnd);
                // console.log("fullCalendar::eventRender() info.event.start:"+moment(info.event.start).utc().format("YYYY/MM/DD HH:mm:ss")+", info.event.end:"+moment(info.event.end).utc().format("YYYY/MM/DD HH:mm:ss"));

                var todayDate = moment(view.currentStart).format("YYYYMMDD");

                var div_description = document.createElement("div");
                var strong_description = document.createElement("strong");
                strong_description.textContent = info.event.extendedProps.description;
                div_description.appendChild(strong_description);
                info.el.appendChild(div_description);

                var div_area_description = document.createElement("div");
                var strong_description = document.createElement("strong");
                // strong_description.textContent = '點名裝置:';
                strong_description.textContent = '課程位置:';
                div_area_description.appendChild(strong_description);

                var span_area = document.createElement("span");
                span_area.setAttribute("class","badge bg-yellow-active");
                span_area.textContent = find_resource_name(info.event.extendedProps.resourceId);
                div_area_description.appendChild(span_area);

                info.el.appendChild(div_area_description);

                // var box_RollCall = document.createElement("div");
                // box_RollCall.setAttribute("class","box box-solid");
                //
                // var boxheader_RollCall = document.createElement("div");
                // boxheader_RollCall.setAttribute("class","box-header");
                //
                // var boxbody_RollCall = document.createElement("div");
                // boxbody_RollCall.setAttribute("class","box-body");
                //
                // box_RollCall.appendChild(boxheader_RollCall);
                // box_RollCall.appendChild(boxbody_RollCall);
                // info.el.appendChild(box_RollCall);

                var span_ShowRollCall = document.createElement("span");
                span_ShowRollCall.setAttribute("class","badge bg-green-active");
                span_ShowRollCall.setAttribute("href","javascript:;");
                span_ShowRollCall.setAttribute("onclick",'showrollcall_click("'+todayDate+'","'+ info.event.id +'");return false;');
                span_ShowRollCall.textContent = '出席狀況';
                info.el.appendChild(span_ShowRollCall);

                var span_DeleteRollCall = document.createElement("span");
                span_DeleteRollCall.setAttribute("class","badge bg-red");
                span_DeleteRollCall.setAttribute("href","javascript:;");
                span_DeleteRollCall.setAttribute("onclick",'deletecourse_click('+project_id+',"'+todayDate+'","'+ info.event.title+'","' + info.event.id +'");return false;');
                span_DeleteRollCall.textContent = '刪除活動';
                info.el.appendChild(span_DeleteRollCall);


                var div_RollCall = document.createElement("div");
                // box_RollCall.setAttribute("class","box box-solid");
                div_RollCall.setAttribute("id",'rollcall_'+info.event.id);
                // var boxbody_RollCall = document.createElement("div");
                // boxbody_RollCall.setAttribute("class","box-body");
                // boxbody_RollCall.setAttribute("id",'rollcall_'+info.event.id);
                // box_RollCall.appendChild(boxbody_RollCall);
                //div_RollCall.setAttribute("id",'rollcall_'+info.event.id);
                info.el.appendChild(div_RollCall);


            }
            // else
            // if (glCalendar.view.type === 'listWeek')
            // {
            //     var todayDate = moment(view.currentStart).format("YYYYMMDD");
            //
            //     var span_ShowRollCall = document.createElement("span");
            //     span_ShowRollCall.setAttribute("class","badge bg-yellow-active");
            //     span_ShowRollCall.setAttribute("href","javascript:;");
            //     span_ShowRollCall.setAttribute("onclick",'showrollcall_click("'+todayDate+'","'+ info.event.id +'");return false;');
            //     span_ShowRollCall.textContent = '出席狀況';
            //     info.el.appendChild(span_ShowRollCall);
            //
            //     var div_RollCall = document.createElement("div");
            //     div_RollCall.setAttribute("id",'rollcall_'+info.event.id);
            //     info.el.appendChild(div_RollCall);
            // }


            // var tooltip = new Tooltip(info.el, {
            //     title: '<div class="bg-green">'+info.event.title+'</div>'+info.event.extendedProps.description+'<div class="bg-red" onclick=showrollcall_click("'+info.event.id+'");return false;>出席狀況</div><dev id=rollcall_'+info.event.id+'></dev>',
            //     html: true,
            //     placement: 'top',
            //     trigger: 'hover',
            //     container: 'body'
            // });
        }
        // eventPositioned: function(info)
        // {
        //     var view = glCalendar.view;
        //     if (view.type === 'listWeek')
        //     {
        //         console.log("fullCalendar::eventPositioned() [listWeek] info.event.id:" + info.event.id + ", title:"+ info.event.title + ", description:"+info.event.extendedProps.description);
        //         console.log("fullCalendar::eventPositioned() [listWeek] info.isStart:" + info.isStart + ", isEnd:"+ info.isEnd);
        //         console.log("fullCalendar::eventPositioned() [listWeek] info.event.start:"+moment(info.event.start).utc().format("YYYY/MM/DD HH:mm:ss")+", info.event.end:"+moment(info.event.end).utc().format("YYYY/MM/DD HH:mm:ss"));
        //
        //         var td_rollcall = document.createElement('td')
        //         info.el.appendChild(td_rollcall);
        //
        //         var span_StartTime = document.createElement("span");
        //         span_StartTime.setAttribute("class","badge bg-yellow-active");
        //         span_StartTime.textContent = info.event.id;
        //         td_rollcall.appendChild(span_StartTime);
        //     }
        // }
    });

    glCalendar.render();

    check_ui_display();

});

function showrollcall_click(showDate, courseId)
{
    console.log("showrollcall_click() showDate:"+showDate+" courseId:"+courseId);

    var div_RollCall = document.getElementById('rollcall_'+courseId);
    div_RollCall.setAttribute("class","box box-solid");
    //div_RollCall.setAttribute("class","bg-teal");
    //div_RollCall.textContent = '出席狀況' + courseId;

    while (div_RollCall.firstChild)
    {
        div_RollCall.removeChild(div_RollCall.firstChild);
    }

    var boxbody_RollCall = document.createElement("div");
    boxbody_RollCall.setAttribute("class","box-body");
    div_RollCall.appendChild(boxbody_RollCall);

    fetch_rollcall_onview(boxbody_RollCall, showDate, courseId);
}


// function DateUTC( curDate )
// {
//     console.log(curDate.toUTCString());
//     // var utc = new Date(curDate.getTime() + curDate.getTimezoneOffset() * 60000);
//     // var utc = new Date(curDate.toUTCString().slice(0, -4));
//     return curDate;
// }

//
// ADDING EVENTS
//
var currColor = '#3c8dbc'; //Red by default

//Color chooser button
var colorChooser = $('#color-chooser-btn');
$('#color-chooser > li > a').click(function (e)
{
    e.preventDefault();
    //Save color
    currColor = $(this).css('color');
    //Add color effect to button
    $('#add-new-event').css({ 'background-color': currColor, 'border-color': currColor });

})

$('#add-new-event').click(function (e)
{
    e.preventDefault();

    //Get value and make sure it is not null
    var title = $('#course_title_InputText').val();
    if (title.length == 0)
    {
        return;
    }
    var description = $('#course_description_InputText').val();

    var allDay = document.getElementById('course_allday_CheckBtn').checked;
    var repeat = document.getElementById('course_repeat_CheckBtn').checked;
    var startTime = document.getElementById('StartTimeInputText').value;
    var endTime = document.getElementById('EndTimeInputText').value;

    if (startTime.length == 4)
    {
        startTime = '0' + startTime ;
    }
    if (endTime.length == 4)
    {
        endTime = '0' + endTime ;
    }

    //var dateRange = document.getElementById('DateRangeInputText').value;
    // var startDate = document.getElementById('DateRangeInputText').data('daterangepicker').startDate;
    // var endDate = document.getElementById('DateRangeInputText').data('daterangepicker').endDate;
    var startDate = $('#DateRangeInputText').data('daterangepicker').startDate.format('YYYY-MM-DD');
    var endDate = $('#DateRangeInputText').data('daterangepicker').endDate.format('YYYY-MM-DD');

    var pickDate = $('#DatePickerInputText').val();

    var startRecur = '';
    var endRecur = '';

    var resourceId = $("#item_show").attr("data-id");
    var courseId = 'C' + moment().format("YYYYMMDDHHmmss");


    if (allDay)
    {
        startTime = '';
        endTime = '';

        if (repeat)
        {
            //  need startDate and endDate
            glCalendar.addEvent({
                resourceId      : resourceId,
                id              : courseId,
                title           : title,
                description     : description,
                allDay          : true,
                start           : startDate,
                end             : endDate,
                backgroundColor : currColor,
                borderColor     : currColor
              });
        }
        else
        {
            // just oneday, default is today
            //var today = moment().format("YYYY-MM-DD");
            startDate = pickDate;
            endDate = pickDate;
            glCalendar.addEvent({
                resourceId      : resourceId,
                id              : courseId,
                title           : title,
                description     : description,
                allDay          : true,
                start           : startDate,
                end             : endDate,
                backgroundColor : currColor,
                borderColor     : currColor
              });
        }
    }
    else
    {
        if (repeat)
        {
            startRecur = startDate;
            endRecur = endDate;
            startDate = '';
            endDate = '';

            // need startTime, endTime, startDate and endDate
            glCalendar.addEvent({
                resourceId      : resourceId,
                id              : courseId,
                title           : title,
                description     : description,
                allDay          : false,
                startTime       : startTime,
                endTime         : endTime,
                startRecur      : startRecur,
                endRecur        : endRecur,
                backgroundColor : currColor,
                borderColor     : currColor
              });
        }
        else
        {
            // need startTime, endTime,
            //var today = moment().format("YYYY-MM-DD");
            // startDate = moment(startTime, 'HH:mm').toDate();
            // endDate = moment(endTime, 'HH:mm').toDate();
            // startDate = moment(startTime, 'HH:mm').format("YYYY-MM-DD HH:mm:ss");
            // endDate = moment(endTime, 'HH:mm').format("YYYY-MM-DD HH:mm:ss");

            var startDateTime = pickDate +' '+startTime;
            var endDateTime = pickDate +' '+endTime;

            startDate = moment(startDateTime, 'YYYY-MM-DD HH:mm').format("YYYY-MM-DD HH:mm:ss");
            endDate = moment(endDateTime, 'YYYY-MM-DD HH:mm').format("YYYY-MM-DD HH:mm:ss");

            //console.log("startDate:"+startDate+", endDate:"+endDate);

            glCalendar.addEvent({
                resourceId      : resourceId,
                id              : courseId,
                title           : title,
                description     : description,
                allDay          : false,
                start           : startDate,
                end             : endDate,
                backgroundColor : currColor,
                borderColor     : currColor
              });
        }
    }

    //
    // send to server
    //
    create_new_course(project_id, resourceId, courseId, title, description,
            allDay, startDate, endDate, startTime, endTime,
            startRecur, endRecur, currColor, currColor);


    //Create events
    // var event = $('<div />');
    // event.css({
    //       'background-color': currColor,
    //       'border-color'    : currColor,
    //       'color'           : '#fff'
    //   }).addClass('external-event');
    //
    // event.html(val);
    //
    // //$('#external-events').prepend(event);
    //
    // //Add draggable funtionality
    // init_events(event);


    // resourceId     : 'd',
    // id             : '1006',
    // title          : 'Click for Google',
    // description    : 'description for Click for Google Event',
    // allDay         : false,
    // start          : new Date(y, m, 28),
    // end            : new Date(y, m, 29),
    // url            : 'http://google.com/',
    // backgroundColor: '#3c8dbc', //Primary (light-blue)
    // borderColor    : '#3c8dbc' //Primary (light-blue)

    // startTime      : '06:00',
    // endTime        : '07:00',
    // daysOfWeek     : [1, 2, 4], // 0:sunday
    // startRecur     : new Date(y, m, 1),
    // endRecur       : new Date(y, m, 30),


    // glCalendar.addEvent({
    //                 resourceId     : '',
    //                 id             : '',
    //                 title: val,
    //                 start: '2019-04-18',
    //                 allDay: allDay,
    //                 backgroundColor: currColor,
    //                 borderColor    : currColor
    //               });

    //Remove event from text input
    //$('#new-event').val('');
});

function find_resource_name(resourceId)
{
    // console.log("find_resource_name() resourceId:"+resourceId + ", glMapAreas.length:"+glMapAreas.length);

    for (var i = 0; i < glMapAreas.length; i++)
    {
        // console.log("find_resource_name() glMapAreas[i].itemid:"+glMapAreas[i].itemid);
        if ( glMapAreas[i].itemid === resourceId)
        {
            return glMapAreas[i].itemname;
        }
    }
    return resourceId;
}


function course_allday_CheckBtn_onclick()
{
    var checked = document.getElementById('course_allday_CheckBtn').checked;

    endable_TimePicker(!checked);

    // if (checked)
    // {
    //     document.getElementById("StartTimePicker").style.display = 'none';
    //     document.getElementById("EndTimePicker").style.display = 'none';
    // }
    // else
    // {
    //     document.getElementById("StartTimePicker").style.display = 'block';
    //     document.getElementById("EndTimePicker").style.display = 'block';
    // }
}

function course_repeat_CheckBtn_onclick()
{
    var checked = document.getElementById('course_repeat_CheckBtn').checked;

    endable_DateRangePicker(checked);

    // if (checked)
    // {
    //     document.getElementById("DateRangePicker").style.display = 'block';
    //     document.getElementById("DatePicker").style.display = 'none';
    // }
    // else
    // {
    //     document.getElementById("DateRangePicker").style.display = 'none';
    //     document.getElementById("DatePicker").style.display = 'block';
    // }
}

function endable_TimePicker(enable)
{
    if (enable)
    {
        document.getElementById("StartTimePicker").style.display = 'block';
        document.getElementById("EndTimePicker").style.display = 'block';
    }
    else
    {
        document.getElementById("StartTimePicker").style.display = 'none';
        document.getElementById("EndTimePicker").style.display = 'none';
    }
}

function endable_DateRangePicker(enable)
{
    if (enable)
    {
        document.getElementById("DateRangePicker").style.display = 'block';
        document.getElementById("DatePicker").style.display = 'none';
    }
    else
    {
        document.getElementById("DateRangePicker").style.display = 'none';
        document.getElementById("DatePicker").style.display = 'block';
    }
}

function check_ui_display()
{
    var course_repeat_CheckBtn = document.getElementById('course_repeat_CheckBtn');
    if ( course_repeat_CheckBtn == undefined )
        return;
    var course_allday_CheckBtn = document.getElementById('course_allday_CheckBtn');
    if ( course_allday_CheckBtn == undefined )
        return;

    var checked = course_repeat_CheckBtn.checked;
    endable_DateRangePicker(checked);

    var checked = course_allday_CheckBtn.checked;
    endable_TimePicker(!checked);
}

function open_courses_page()
{
    var win = window.open('/index.php/vilscourses/'+project_id, '_blank');
    win.focus();
}
