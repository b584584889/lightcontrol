/*
 * Author: ikki Chung
 * Date: 7 Sep 2017
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


function TagFootPrintCacheData()
{
    this.node_id = "";
    this.node_name = "";

    this.scale_start_date = "";
    this.scale_end_date = "";
    this.scale_footprint = [];

    this.playback_start_date = "";
    this.playback_end_date = "";
    this.playback_footprint = [];

    this.updating = false;


    //this.load_scale_footprint = function(tagid, uiItems, timeScale, queryStartDate, queryEndDate)
    this.load_sampling_footprint = function(tagid, uiItems, sampling, queryStartDate, queryEndDate, load_footprint_cb)
    {
        //var queryStartDate = '2017-09-04 12:00:00';
        //var queryEndDate = '2017-09-07 12:00:00';
        this.scale_start_date = queryStartDate;
        this.scale_end_date = queryEndDate;
        this.updating = true;
        //alert("queryStartDate:"+queryStartDate + ", glRangeStartDate:"+glRangeStartDate);

        var count = 0;
        var startDate = new Date(); // Current date now.
        var endDate = new Date(); // Current date now.
        var preArea = "";
        var queryString = 'loadtag='+tagid+'&sampling='+sampling+'&startdate='+queryStartDate+'&enddate='+queryEndDate+'&project_id='+project_id;

        this.scale_footprint = [];
        var _this = this;

        var targetURL = gl_target_server + "/php/fpnodes.php";
        $.getJSON(targetURL, encodeURI(queryString), function(data)
        {

            $.each(data, function(ftkey, value)
            {
                var curDate = moment(value['datetime']).format('YYYY-MM-DD HH:mm:ss');

                _this.scale_footprint[curDate] = {'X': value['X'],'Y': value['Y'],'Z': value['Z'],'v': value['v'],'t': value['t']};

                if (count == 0)
                {
                    startDate = new Date(value['datetime']);
                    endDate = new Date(value['datetime']);
                    if (value['area'] != null)
                    {
                        preArea = value['area'];
                    }
                    else
                        preArea = "";
                    //alert("preArea="+preArea);
                }
                else
                {
                    var diffSeconds = parseInt((value['datetime'] - endDate) / 1000);
                    var diffRangeSeconds = parseInt((endDate - startDate) / 1000);
                    //var diffSeconds = endDate.diff(startDate, 'seconds');// moment.js
                    // if (count == 1)
                    // {
                    //     alert(nodeid + ": diffSeconds="+diffSeconds);
                    // }
                    var curArea = "";
                    if (value['area'] != null)
                    {
                        curArea = value['area'];
                    }

                    // if ( count == data.length-1 )
                    // {
                    //     alert("count:"+count);
                    // }


                    //if ( diffSeconds >= glTimeScale || (preArea != curArea) || diffRangeSeconds >= glTimeScale)
                    if ( (preArea != curArea) || (count == data.length-1) )
                    {
                        uiItems.add(
                            {start:startDate,
                            end:endDate,
                            group:tagid,
                            content:preArea,
                            //title:moment(startDate).format('MMMM Do YYYY, h:mm:ss a')+"-->"+moment(endDate).format('MMMM Do YYYY, h:mm:ss a')
                            title:preArea,
                            type: "box"
                        });

                        startDate = new Date(value['datetime']);
                        endDate = new Date(value['datetime']);
                    }
                    else
                    {
                        endDate = new Date(value['datetime']);
                    }
                    preArea = curArea;
                }

                //start: new Date(2017, 5, 10),
                //end: new Date(2017, 5, 11),
                //group:"Tag024",
                //className:"openwheel",
                //content:"Argentina",
                //id:"531@motocal.net",
                //title:"531@motocal.net"
                //var rangings = JSON.parse(value);
                // items.add(
                //     {start:value['datetime'],
                //     end:value['datetime'],
                //     group:nodeid,
                //     title:ftkey+":"+value['datetime']
                // });
                // if (count == 0)
                // {
                //     alert(nodeid + ": ftkey="+ftkey + ": value="+value['datetime']);
                // }

                count++;
            });
        })
        .complete(function() {

            uiItems.add(
                {start:startDate,
                end:endDate,
                group:tagid,
                content:preArea,
                //title:moment(startDate).format('MMMM Do YYYY, h:mm:ss a')+"-->"+moment(endDate).format('MMMM Do YYYY, h:mm:ss a')
                title:preArea,
                type: "box"
           });

           //
           // assign to global
           //
           this.scale_footprint = _this.scale_footprint;
           this.updating = false;

           load_footprint_cb(tagid, queryStartDate, queryEndDate);
        });
        return false;
    }

    this.getFootPrint = function(curDate)
    {
        var value = null;
        var found = 0
        var ftdate;
        for(ftdate in this.scale_footprint)
        {
            //var nodeDate = new Date(ftdate);

            if ( curDate >= ftdate )
            {
                found = 1;
                // next is our
                continue;
            }

            if (found == 1)
            {
                value = this.scale_footprint[ftdate];
                break;
            }
        }

        return value;
    }

    //
    // curDate format('YYYY-MM-DD HH:mm:ss')
    //
    this.getPlayBackFootPrint = function(curDate)
    {
        var value = null;
        //
        // check time range
        //
        if ( this.playback_start_date == "" ||
             curDate < this.playback_start_date ||
             curDate > this.playback_end_date)
        {

            //alert( this.node_id +" getPlayBackFootPrint() curDate="+curDate+", this.playback_start_date="+this.playback_start_date+", this.playback_end_date="+this.playback_end_date);

            //
            // fetch new data
            //
            // get 1 hour data
            var start = new Date(new Date(curDate).getTime() - 30 * 60 * 1000);
            var end   = new Date(new Date(curDate).getTime() + 30 * 60 * 1000);
            this.updateCache(start, end);
        }

        var found = 0
        var ftdate;
        for(ftdate in this.playback_footprint)
        {
            //var nodeDate = new Date(ftdate);

            if ( curDate >= ftdate )
            {
                found = 1;
                // next is our
                continue;
            }

            if (found == 1)
            {
                value = this.playback_footprint[ftdate];
                break;
            }
        }

        return value;
    }

    //
    // update footprint cache
    //
    this.updateCache = function(startDate, endDate)
    {
        var queryStartDate = formatDate(startDate);
        var queryEndDate = formatDate(endDate);

        //alert( this.node_id +" updateCache() strStartDate="+strStartDate+", strEndDate="+strEndDate);

        //var queryString = 'loadtag='+this.node_id+'&scale='+1+'&startdate='+queryStartDate+'&enddate='+queryEndDate;
        var queryString = 'loadtag='+this.node_id+'&sampling=1&startdate='+queryStartDate+'&enddate='+queryEndDate+'&project_id='+project_id;

        //var d = new Date();
        //console.log(this.node_id+": updateCache():"+ d.getSeconds()+":"+ d.getMilliseconds());

        this.updating = true;
        this.playback_footprint = [];
        var _this = this;

        var targetURL = gl_target_server + "/php/fpnodes.php";
        $.getJSON(targetURL, encodeURI(queryString), function(data)
        {
            $.each(data, function(ftkey, value)
            {
                var curDate = moment(value['datetime']).format('YYYY-MM-DD HH:mm:ss');

                _this.playback_footprint[curDate] = {'X': value['X'],'Y': value['Y'],'Z': value['Z'],'v': value['v'],'t': value['t']};
            });
        })
        .complete(function() {
            _this.playback_start_date = queryStartDate;
            _this.playback_end_date = queryEndDate;
            _this.updating = false;

            //var d = new Date();
            //console.log(_this.node_id+": updateCache(): finished!, "+ d.getSeconds()+":"+ d.getMilliseconds());

        });
        return false;
    }

    //
    // convert Date to string format YYYY-MM-DD hh:mm:ss
    //
    function formatDate(targeDate)
    {
        var strDate = targeDate.getFullYear()+'-'
            +(""+ (targeDate.getMonth() + 101)).substr(1)+'-'
            +(""+ (targeDate.getDate() + 100)).substr(1)+' '
            +(""+ (targeDate.getHours() + 100)).substr(1)+':'
            +(""+ (targeDate.getMinutes() + 100)).substr(1)+':'
            +(""+ (targeDate.getSeconds() + 100)).substr(1);
        return strDate;
    }
}
