



update_app_version_and_pid_loop();
load_All_config();
load_system_time();
// load_websocket();

function load_All_config()
{
    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadallconfig=1'+'&project_id='+project_id , function(data)
    {
        var prjconfigs = [];

        $.each(data, function(prjid, value)
        {
            //prjids.push(prjid);
            //var prjconfig = value;
            prjconfigs[parseInt(prjid)]=value;
            //alert("prjid=" + prjid +", project_id="+project_id);
        });

        //alert("prjids.length=" + prjids.length+", prjconfigs.length=" + prjconfigs.length);

        var curConfig = prjconfigs[project_id];
        if (curConfig != null)
            document.getElementById("curProjectName").innerHTML = " " + curConfig.PROJECT_NAME;
        else
            document.getElementById("curProjectName").innerHTML = " ";

        var prjNum = Object.keys(prjconfigs).length;
        document.getElementById("totalProjects").innerHTML = "You have "+ prjNum +" projects";

        document.getElementById("curUserName").innerHTML = " " + accountname;

        for (var key in prjconfigs)
        {
            var config = prjconfigs[key];
            add_project_to_menu(config);
        }

        //setTimeout(check_tag_alert, 1000);
    });
}

function fetch_current_app_version()
{
    //document.getElementById("appversion").innerText = "Fetching Version...";
    var targetURL = gl_target_server + "/php/pkgversion.php";
    $.getJSON(targetURL, 'project_id='+project_id, function(data)
    {
        document.getElementById("appversion").innerText = data.version;
        document.getElementById("apppid").innerText = 'PID:'+data.pid;
    });
    return false;
}

function sync_ERP_account()
{
    var SendDate = "syncERPaccount=1&project_id="+project_id;
    var targetURL = gl_target_server + "/login.php";

    console.log("sync_ERP_account() "+targetURL);

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
            console.log("sync_ERP_account() response:"+response);

        }
    });

    return false;
}

function get_qrcode(accountid, userid)
{
    var SendDate = "loadqrcode=1&type=USER&iaccountid=" + accountid+"&id=" + userid+'&project_id='+project_id;
    var targetURL = gl_target_server + "/php/configvils.php";

    console.log("get_qrcode() "+targetURL);

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
            //console.log("sync_ERP_account() response:"+response);

            var result = JSON.parse(response);
            //console.log("sync_ERP_account() success:"+result.success+" qrcode:"+result.qrcode);

            var qrcode = document.getElementById("qrcode_ui");
            qrcode.setAttribute("src", result.qrcode);

        }
    });

    return false;
}

function get_pdf_report(accountid)
{
    var SendDate = "loadpdfreport=1&type=USER&id=" + accountid+'&project_id='+project_id;
    var targetURL = gl_target_server + "/php/configvils.php";

    console.log("get_pdf_report() "+targetURL);

    var pdf_report_dl_link = document.getElementById("pdf_report_dl_link");
    while (pdf_report_dl_link.firstChild)
    {
        pdf_report_dl_link.removeChild(pdf_report_dl_link.firstChild);
    }
    var loading = document.createElement('span');
    loading.textContent = "產生報表中...";
    pdf_report_dl_link.appendChild(loading);


    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
            //console.log("sync_ERP_account() response:"+response);

            var result = JSON.parse(response);
            console.log("get_pdf_report() success:"+result.success+" pdfreport:"+result.pdfreport);

            var pdf_report_dl_link = document.getElementById("pdf_report_dl_link");
            while (pdf_report_dl_link.firstChild)
            {
                pdf_report_dl_link.removeChild(pdf_report_dl_link.firstChild);
            }

            var dl_link = document.createElement("a");
            dl_link.setAttribute("href",result.pdfreport);
            dl_link.setAttribute("target","_blank");
            dl_link.textContent = "下載報表";
            pdf_report_dl_link.appendChild(dl_link);

            var imgPdf = document.createElement("i");
            imgPdf.setAttribute("class","fa fa-file-pdf-o");
            dl_link.appendChild(imgPdf);

            //var qrcode = document.getElementById("qrcode_ui");
            //qrcode.setAttribute("src", result.qrcode);

        }
    });

    return false;
}

function add_project_to_menu(config)
{
    if (config == null)
        return;

    var prjmenu = document.getElementById('ProjectListMenu');

    var li = document.createElement('li');
    prjmenu.appendChild(li);

    var a = document.createElement('a');
    a.setAttribute("href","/index.php/"+parseInt(config.PROJECT_ID));
    li.appendChild(a);

    var div = document.createElement("div");
    div.setAttribute("class","pull-left");
    a.appendChild(div);

    var img = document.createElement("img");
    img.setAttribute("src","/images/VILS_logo_m.png");
    img.setAttribute("class","img-circle");
    img.setAttribute("alt","User Image");
    div.appendChild(img);

    var prjname = document.createElement("h4");
    prjname.textContent = config.PROJECT_NAME;
    a.appendChild(prjname);

    var prjdesc = document.createElement("p");
    prjdesc.textContent = config.PROJECT_DESCRIPTION;
    a.appendChild(prjdesc);

}

function create_new_project(obj)
{
    //alert(obj.getAttribute("href"));
    var jsondata = {};
    jsondata['create_new_project'] = 1;
    var targetURL = gl_target_server + "/php/configvils.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: jsondata,
      success:
        function(response)
        {
             console.log("response="+response);
             //alert("response="+response);
             var config = JSON.parse(response);

             window.location = "/index.php/"+parseInt(config.PROJECT_ID);

             // if ( result.create === 'success' )
             // {
             //     window.location = "/index.php/"+parseInt(config.PROJECT_ID);
             // }
             // else
             // if ( result.create === 'error' )
             // {
             //    alert("Create new project Error!");
             // }
             // else
             // if ( result.create === 'failed' )
             // {
             //     alert("Create new project failed!");
             // }

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

function show_dashboard(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsdashboard/"+project_id;
    show_treeview('dashboard');
}

function show_devices(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsdevices/"+project_id;
    show_treeview('devices');
}

function show_rfiddevices(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsrfiddevices/"+project_id;
    show_treeview('rfiddevices');
}

function show_rfidcards(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsrfidcards/"+project_id;
    show_treeview('rfiddevices');
}

function show_userinfo(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsuserinfo/"+project_id;
    show_treeview('userinfo');
}

function show_userregister(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsuserregister/"+project_id;
    show_treeview('userinfo');
}

function show_usergroupinfo(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsusergroupinfo/"+project_id;
    show_treeview('userinfo');
}

function show_usersubgroup_unlockpage(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsunlockpage/"+project_id;
    show_treeview('userinfo');
}

function show_usersubgroupinfo(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsusersubgroupinfo/"+project_id;
    show_treeview('userinfo');
}

function show_usermaingroupinfo(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsusermaingroupinfo/"+project_id;
    show_treeview('userinfo');
}

function show_carddrollcall_schedule(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilscardrollcall/"+project_id;
    show_treeview('cardrollcall');
}

function show_cardrollcall_event(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilscardrollcallevent/"+project_id;
    show_treeview('cardrollcall');
}

function show_carddrollcall_schedule_unlockpage(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsunlockpage/"+project_id;
    show_treeview('cardrollcall');
}

function show_cardrollcall_event_unlockpage(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsunlockpage/"+project_id;
    show_treeview('cardrollcall');
}

function show_cardrollcall_record(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilscardrollcallrecord/"+project_id;
    show_treeview('cardrollcall');
}

function show_userrollcall_record(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsuserrollcallrecord/"+project_id;
    show_treeview('cardrollcall');
}

function show_nfc_cardscan_result(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilscardscannfcresult/"+project_id;
    show_treeview('cardscanresult');
}

function show_uhf_cardscan_result(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilscardscanuhfresult/"+project_id;
    show_treeview('cardscanresult');
}

function show_nfc_cardscan_result_unlockpage(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsunlockpage/"+project_id;
    show_treeview('cardscanresult');
}

function show_uhf_cardscan_result_unlockpage(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsunlockpage/"+project_id;
    show_treeview('cardscanresult');
}

function show_bodyinfo(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsbodyinfo/"+project_id;
    show_treeview('bodyinfo');
}

function show_bodyinfo_record(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsbodyinforecord/"+project_id;
    show_treeview('bodyinfo');
}

function show_bodyinfo_manual_input(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsbodyinfomanualinput/"+project_id;
    show_treeview('bodyinfo');
}

function show_foracare(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsbodyinfoforacare/"+project_id;
    show_treeview('bodyinfo');
}

function show_pushnotification(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilspushnotification/"+project_id;
    show_treeview('pushnotification');
}

function show_pushnotification_records(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilspushnotificationrecords/"+project_id;
    show_treeview('pushnotification');
}

function show_survey(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilssurvey/"+project_id;
    show_treeview('workassign');
}

function show_survey_records(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilssurveyrecords/"+project_id;
    show_treeview('workassign');
}

function show_assist(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsassist/"+project_id;
    show_treeview('workassign');
}

function show_assist_records(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsassistrecords/"+project_id;
    show_treeview('workassign');
}

function show_billboard(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsbillboard/"+project_id;
    show_treeview('billboard');
}

function show_billboard_records(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsbillboardrecords/"+project_id;
    show_treeview('billboard');
}

function show_curriculum(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilscurriculum/"+project_id;
    show_treeview('curriculum');
}

function show_curriculum_records(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilscurriculumrecords/"+project_id;
    show_treeview('curriculum');
}

function show_location()
{
    document.getElementById('contentIframe').src = "/index.php/vilslocation/"+project_id;
    show_treeview('location');
}

function show_ranging(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsranging/"+project_id;
    show_treeview('ranging');
}

function show_2dmap(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vils2dmap/"+project_id;
    show_treeview('2dmap');
}

function show_3dmap(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vils3dmap/"+project_id;
    show_treeview('3dmap');
}

function show_fieldcontrol(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsfieldcontrol/"+project_id;
    show_treeview('fieldcontrol');
}

function show_firefighting_create(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsfirefightingcreate/"+project_id;
    show_treeview('firefighting');
}

function show_firefighting_record(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsfirefightingrecord/"+project_id;
    show_treeview('firefighting');
}

function show_firefighting_history(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsfirefightinghistory/"+project_id;
    show_treeview('firefighting');
}

function show_firefighting_report(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsfirefightingreport/"+project_id;
    show_treeview('firefighting');
}

function show_versionhistory(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsversionhistory/"+project_id;
    show_treeview('versionhistory');
}

function show_activity(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsactivity/"+project_id;
    show_treeview('activity');
}

function show_maintenance(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsmaintenance/"+project_id;
    show_treeview('maintenance');
}

function show_events(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsevents/"+project_id;
    show_treeview('events');
}

function show_courses(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilscourses/"+project_id;
    show_treeview('courses');
}

function show_weekschedule(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsweekschedule/"+project_id;
    show_treeview('weekschedule');
}

function show_applicationform_leave(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsapplicationformleave/"+project_id;
    show_treeview('applicationform');
}

function show_footprint_tags(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsfootprinttags/"+project_id;
    show_treeview('footprint');
}

function show_footprint_Courses(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsfootprintcourses/"+project_id;
    show_treeview('footprint');
}

function show_footprint_Areas(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsfootprintareas/"+project_id;
    show_treeview('footprint');
}

function show_footprint_InOutArea(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsfootprintinout/"+project_id;
    show_treeview('footprint');
}

function show_footprint_GPS(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsfootprintgps/"+project_id;
    show_treeview('footprint');
}

function show_footprint(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsfootprint/"+project_id;
    show_treeview('footprint');
}

function show_eventtimeline(projectid)
{
    document.getElementById('contentIframe').src = "/index.php/vilseventtimeline/"+projectid;
    show_treeview('eventtimeline');
}

function show_eventReport(projectid)
{
    document.getElementById('contentIframe').src = "/index.php/vilseventreport/"+projectid;
    show_treeview('eventreport');
}

function show_powermeterview(projectid)
{
    document.getElementById('contentIframe').src = "/index.php/vilspowermeterview/"+projectid;
    show_treeview('powermeterview');
}

function show_lightdevicecontrol(projectid)
{
    document.getElementById('contentIframe').src = "/index.php/vilslightdevicecontrol/"+projectid;
    show_treeview('lightdevicecontrol');
}

function show_watchlocation(projectid)
{
    document.getElementById('contentIframe').src = "/index.php/vilswatchlocation/" + projectid;
    show_treeview('vilswatchlocation');
}

// function show_checkpoint_setting(project_id)
// {
//     document.getElementById('contentIframe').src = "/index.php/vilscheckpoint/"+project_id;
//     show_treeview('checkpoint');
// }
//
// function show_checkpoint_record(project_id)
// {
//     document.getElementById('contentIframe').src = "/index.php/vilscheckpointrecord/"+project_id;
//     show_treeview('checkpoint');
// }
//
// function show_checkpoint_history(project_id)
// {
//     document.getElementById('contentIframe').src = "/index.php/vilscheckpointhistory/"+project_id;
//     show_treeview('checkpoint');
// }

function show_inspection_report(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsinspectionreport/"+project_id;
    show_treeview('inspection');
}

function show_inspection_setting(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsinspection/"+project_id;
    show_treeview('inspection');
}

function show_inspection_record(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsinspectionrecord/"+project_id;
    show_treeview('inspection');
}

function show_inspection_history(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsinspectionhistory/"+project_id;
    show_treeview('inspection');
}

function show_sections(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilssections/"+project_id;
    show_treeview('sections');
}

function show_fencealert(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsfencealert/"+project_id;
    show_treeview('fencealert');
}

function show_filestorage_image(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsstorageimage/"+project_id;
    show_treeview('filestorage');
}

function show_filestorage_file(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsstoragefile/"+project_id;
    show_treeview('filestorage');
}

function show_settings(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilssettings/"+project_id;
    show_treeview('settings');
}

function show_disetting(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsdisetting/"+project_id;
    show_treeview('disetting');
}

function show_pushnotify(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilspushnotify/"+project_id;
    show_treeview('pushnotify');
}

function show_diagnosis(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsdiagnosis/"+project_id;
    show_treeview('diagnosis');
}

function show_analyzer(project_id)
{
    document.getElementById('contentIframe').src = "/index.php/vilsanalyzer/"+project_id;
    show_treeview('analyzer');
}

function show_treeview(type)
{
    if (document.getElementById('treeview_dashboard') != null)
        document.getElementById('treeview_dashboard').classList.remove('active');
    if (document.getElementById('treeview_devices') != null)
        document.getElementById('treeview_devices').classList.remove('active');
    if (document.getElementById('treeview_rfiddevices') != null)
        document.getElementById('treeview_rfiddevices').classList.remove('active');
    if (document.getElementById('treeview_rfiddevices') != null)
        document.getElementById('treeview_userinfo').classList.remove('active');
    if (document.getElementById('treeview_cardrollcall') != null)
        document.getElementById('treeview_cardrollcall').classList.remove('active');
    if (document.getElementById('treeview_cardscanresult') != null)
        document.getElementById('treeview_cardscanresult').classList.remove('active');
    if (document.getElementById('treeview_bodyinfo') != null)
        document.getElementById('treeview_bodyinfo').classList.remove('active');
    if (document.getElementById('treeview_pushnotification') != null)
        document.getElementById('treeview_pushnotification').classList.remove('active');
    if (document.getElementById('treeview_location') != null)
        document.getElementById('treeview_location').classList.remove('active');
    if (document.getElementById('treeview_ranging') != null)
        document.getElementById('treeview_ranging').classList.remove('active');
    if (document.getElementById('treeview_2dmap') != null)
        document.getElementById('treeview_2dmap').classList.remove('active');
    if (document.getElementById('treeview_3dmap') != null)
        document.getElementById('treeview_3dmap').classList.remove('active');
    if (document.getElementById('treeview_fieldcontrol') != null)
        document.getElementById('treeview_fieldcontrol').classList.remove('active');
    if (document.getElementById('treeview_firefighting') != null)
        document.getElementById('treeview_firefighting').classList.remove('active');
    if (document.getElementById('treeview_versionhistory') != null)
        document.getElementById('treeview_versionhistory').classList.remove('active');
    if (document.getElementById('treeview_activity') != null)
        document.getElementById('treeview_activity').classList.remove('active');
    if (document.getElementById('treeview_maintenance') != null)
        document.getElementById('treeview_maintenance').classList.remove('active');
    if (document.getElementById('treeview_events') != null)
        document.getElementById('treeview_events').classList.remove('active');
    if (document.getElementById('treeview_workassign') != null)
        document.getElementById('treeview_workassign').classList.remove('active');
    if (document.getElementById('treeview_billboard') != null)
        document.getElementById('treeview_billboard').classList.remove('active');
    if (document.getElementById('treeview_curriculum') != null)
        document.getElementById('treeview_curriculum').classList.remove('active');
    if (document.getElementById('treeview_courses') != null)
        document.getElementById('treeview_courses').classList.remove('active');
    if (document.getElementById('treeview_weekschedule') != null)
        document.getElementById('treeview_weekschedule').classList.remove('active');
    if (document.getElementById('treeview_applicationform') != null)
        document.getElementById('treeview_applicationform').classList.remove('active');
    if (document.getElementById('treeview_footprint') != null)
        document.getElementById('treeview_footprint').classList.remove('active');
    // if (document.getElementById('treeview_checkpoint') != null)
    //     document.getElementById('treeview_checkpoint').classList.remove('active');
    if (document.getElementById('treeview_inspection') != null)
        document.getElementById('treeview_inspection').classList.remove('active');
    if (document.getElementById('treeview_sections') != null)
        document.getElementById('treeview_sections').classList.remove('active');
    if (document.getElementById('treeview_fencealert') != null)
        document.getElementById('treeview_fencealert').classList.remove('active');
    if (document.getElementById('treeview_filestorage') != null)
        document.getElementById('treeview_filestorage').classList.remove('active');
    if (document.getElementById('treeview_settings') != null)
        document.getElementById('treeview_settings').classList.remove('active');
    if (document.getElementById('treeview_disetting') != null)
        document.getElementById('treeview_disetting').classList.remove('active');
    if (document.getElementById('treeview_diagnosis') != null)
        document.getElementById('treeview_diagnosis').classList.remove('active');
    if (document.getElementById('treeview_analyzer') != null)
        document.getElementById('treeview_analyzer').classList.remove('active');
    if (document.getElementById('treeview_pushnotify') != null)
        document.getElementById('treeview_pushnotify').classList.remove('active');
    if (document.getElementById('treeview_scotttest') != null)
        document.getElementById('treeview_scotttest').classList.remove('active');

    //document.getElementById("rightcontentIframe").style.display = "none";
    //document.getElementById("rightcontentIframe").style.display = "block";
    //document.getElementById("rightcontentIframe").style.visibility = "visible";
    var main_left_frame = document.getElementById("main_left_frame");
    var main_right_frame = document.getElementById("main_right_frame");
    var show_right_frame = true;

    if (type == 'dashboard')
    {
        console.log("show_treeview() type == 'dashboard'");
        document.getElementById('treeview_dashboard').classList.add('active');
    }
    if (type == 'devices')
    {
        document.getElementById('treeview_devices').classList.add('active');
    }
    if (type == 'rfiddevices')
    {
        document.getElementById('treeview_rfiddevices').classList.add('active');
    }
    if (type == 'userinfo')
    {
        document.getElementById('treeview_userinfo').classList.add('active');
    }
    if (type == 'cardrollcall')
    {
        document.getElementById('treeview_cardrollcall').classList.add('active');
    }
    if (type == 'cardscanresult')
    {
        document.getElementById('treeview_cardscanresult').classList.add('active');
    }
    if (type == 'bodyinfo')
    {
        document.getElementById('treeview_bodyinfo').classList.add('active');
    }
    if (type == 'pushnotification')
    {
        document.getElementById('treeview_pushnotification').classList.add('active');
    }
    if (type == 'location')
    {
        document.getElementById('treeview_location').classList.add('active');
    }
    if (type == 'ranging')
    {
        document.getElementById('treeview_ranging').classList.add('active');
    }
    if (type == '2dmap')
    {
        document.getElementById('treeview_2dmap').classList.add('active');
    }
    if (type == '3dmap')
    {
        document.getElementById('treeview_3dmap').classList.add('active');
    }
    if (type == 'fieldcontrol')
    {
        document.getElementById('treeview_fieldcontrol').classList.add('active');
    }
    if (type == 'firefighting')
    {
        document.getElementById('treeview_firefighting').classList.add('active');
    }
    if (type == 'versionhistory')
    {
        document.getElementById('treeview_versionhistory').classList.add('active');
    }
    if (type == 'activity')
    {
        document.getElementById('treeview_activity').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'maintenance')
    {
        document.getElementById('treeview_maintenance').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'events')
    {
        document.getElementById('treeview_events').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'courses')
    {
        document.getElementById('treeview_courses').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'workassign')
    {
        document.getElementById('treeview_workassign').classList.add('active');
    }
    if (type == 'billboard')
    {
        document.getElementById('treeview_billboard').classList.add('active');
    }
    if (type == 'curriculum')
    {
        document.getElementById('treeview_curriculum').classList.add('active');
    }
    if (type == 'weekschedule')
    {
        document.getElementById('treeview_weekschedule').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'applicationform')
    {
        document.getElementById('treeview_applicationform').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'footprint')
    {
        document.getElementById('treeview_footprint').classList.add('active');
        show_right_frame = false;
    }
    // if (type == 'checkpoint')
    // {
    //     document.getElementById('treeview_checkpoint').classList.add('active');
    //     show_right_frame = false;
    // }
    if (type == 'inspection')
    {
        document.getElementById('treeview_inspection').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'sections')
    {
        document.getElementById('treeview_sections').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'fencealert')
    {
        document.getElementById('treeview_fencealert').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'filestorage')
    {
        document.getElementById('treeview_filestorage').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'settings')
    {
        document.getElementById('treeview_settings').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'disetting')
    {
        document.getElementById('treeview_disetting').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'pushnotify')
    {
        document.getElementById('treeview_pushnotify').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'diagnosis')
    {
        document.getElementById('treeview_diagnosis').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'analyzer')
    {
        document.getElementById('treeview_analyzer').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'eventreport')
    {
        document.getElementById('treeview_scotttest').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'powermeterview')
    {
        document.getElementById('treeview_scotttest').classList.add('active');
        show_right_frame = false;
    }
    if (type == 'lightdevicecontrol')
    {
        document.getElementById('treeview_scotttest').classList.add('active');
        show_right_frame = false;
    }

    if (show_right_frame == false)
    {
        main_left_frame.setAttribute("class","col-md-12 col-sm-12");
        main_right_frame.setAttribute("class","col-md-0 col-sm-0");
        document.getElementById("rightcontentIframe").style.display = "none";
    }
    else
    {
        main_left_frame.setAttribute("class","col-md-8 col-sm-8");
        main_right_frame.setAttribute("class","col-md-4 col-sm-4");
        document.getElementById("rightcontentIframe").style.display = "block";
    }

}

function send_pushnotification(projectid, nodeid, title, message)
{
    document.getElementById('contentIframe').src = "/index.php/vilspushnotification/"+project_id+"?nodeid="+nodeid+"&title="+title+"&message="+message;
    show_treeview('pushnotification');
}

function update_app_version_and_pid_loop()
{
    fetch_current_app_version();

    setTimeout(update_app_version_and_pid_loop, 10000);
}


var gl_system_time;
function update_system_time()
{
    gl_system_time.add(1, 'seconds');
    document.getElementById("system_time").innerHTML = gl_system_time.format('YYYY/MM/DD HH:mm:ss');

    setTimeout(update_system_time, 1000);
}

function load_system_time()
{
    var targetURL = gl_target_server + "/php/pkgversion.php";
    $.getJSON(targetURL, 'fetchsystemtime=1', function(data)
    {
        //gl_system_time = moment(data.systemtime).format('YYYY/MM/DD HH:mm::ss');
        gl_system_time = moment(data.systemtime);

        document.getElementById("system_time").innerHTML = gl_system_time.format('YYYY/MM/DD HH:mm:ss');

    }).complete(function() {
        setTimeout(update_system_time, 1000);
    });
}

var refreshSession = function ()
{
    var targetURL = gl_target_server + "/php/refresh_session.php";

    var time = 600000; // 10 mins
    setTimeout(function (){
        $.ajax({
               url: targetURL,
               cache: false,
               complete: function () {refreshSession();}
            });
        },
        time
    );
};

// Call in page
refreshSession();
