/*
 * Author: ikki Chung
 * Date: 28 Dec 2016
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

 //
 // click handle
 //
 function menulick(maketype, markindex)
 {
     console.log("menulick() maketype = " + maketype + ", markindex=" + markindex);
     curURl = window.location.href;
     if (curURl.indexOf("/events") >= 0)
         window.location = curURl.replace("/events", "");
     else
     if (curURl.indexOf("/activities") >= 0)
         window.location = curURl.replace("/activities", "");
     else
     {
         // global value glLiveMap
         if (window.glLiveMap)
         {
             window.glLiveMap.markclick(maketype, markindex);
         }
     }
}



//
// check
//
function checkProjectActivity()
{
    var mrefreshinterval = 5000; // update display every 5 seconds

    var mUpdate = function ()
    {
        //
        // API: /api/v1/nodes
        //
        $.getJSON('/api/v1/users/'+user_id+'/projects/'+project_id+'/nodes/RPI3/alive',
        function(event)
        {
            if ( event.updatedate == null )
                return ;


            var dateFormat = 'YYYY-MM-DD HH:mm:ss';
            var utc_date = moment.utc(event.updatedate);
            var local_date = utc_date.local();
            //var cur_date = local_date.format(dateFormat);

            var now = moment();
            var ms = now.diff(local_date);
            var d = moment.duration(ms);
            //var s = d.format("hh:mm:ss");
            var hours = d.hours();
            var minutes = d.minutes();
            var seconds = d.seconds();
            //var milliseconds = duration.milliseconds();
            //s = ''+hours+':'+minutes+':'+seconds;

            var live = false;
            if (hours <= 0 && minutes < 2)
            {
                live = true;
                //alert("checkProjectActivity live hours[" + hours + "] minutes[" + minutes + "] seconds[" + seconds + "] local_date[" + local_date+"]");
            }

            // left menu
            if ( live )
            {
                document.getElementById("time-ctrl-live-label").innerHTML = "LIVE";
                document.getElementById("time-ctrl-live-label").className = "label label-danger pull-right-container";
            }
            else
            {
                document.getElementById("time-ctrl-live-label").innerHTML = "Off Line";
                document.getElementById("time-ctrl-live-label").className = "label bg-yellow pull-right-container";
            }
        });
        setTimeout(mUpdate, mrefreshinterval);
     };

     mUpdate();
}

// start run
//setTimeout(checkProjectActivity, 10000);
