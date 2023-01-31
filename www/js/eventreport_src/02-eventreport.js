
var LOAD_PAGE_NUM = 10;
var LOAD_STATE_END = 9999;
var PreLoadIndex = -1;
var CurrentLoadIndex = 0;

var gl_usergroups = [];
var gl_userdatasubgroups = [];

var gl_other_CurrentPage = 1;
var gl_light_CurrentPage = 1;
var gl_uw_CurrentPage = 1;
var gl_watch_CurrentPage = 1;
var gl_pm_CurrentPage = 1;

var gl_other_event = [];
var gl_light_event = [];
var gl_watch_event = [];
var gl_uw_event = [];
var gl_pm_event = [];

var gl_other_line_chart = null;
var gl_light_line_chart = null;
var gl_uw_line_chart = null;
var gl_watch_line_chart = null;
var gl_pm_line_chart_v = null;
var gl_pm_line_chart_w = null;

const tabKey =
{
    tab_other: 'other',
    tab_light: 'light',
    tab_uw: 'uw',
    tab_watch: 'watch',
    tab_pm: 'pm'
}

check_loading_status();

function check_loading_status()
{

  switch (CurrentLoadIndex) {
    case 0: //群組
      if (PreLoadIndex != CurrentLoadIndex)
      {
        console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
        PreLoadIndex = CurrentLoadIndex;
        load_usergroup();
      }
      break;
    case 1: //子集合
      if (PreLoadIndex != CurrentLoadIndex)
      {
        console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
        PreLoadIndex = CurrentLoadIndex;
        load_userdatasubgroup();
      }
      break;
    case 2: //載入資料
      if (PreLoadIndex != CurrentLoadIndex)
      {
        console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
        PreLoadIndex = CurrentLoadIndex;

        load_otherEvnet_records();
        load_lightEvent_records();
        load_uwEvent_records();
        load_watchEvent_records();
        load_pmEvent_records();
        CurrentLoadIndex++;
      }
      break;
    case 3: //佈置資料
      if (PreLoadIndex != CurrentLoadIndex)
      {
        console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
        PreLoadIndex = CurrentLoadIndex;
      }
      break;
    case 4: //結束載入階段
      if (PreLoadIndex != CurrentLoadIndex)
      {
        console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
        PreLoadIndex = CurrentLoadIndex;

        CurrentLoadIndex = LOAD_STATE_END;
      }
      break;
  }

  if (CurrentLoadIndex != LOAD_STATE_END)
    setTimeout(check_loading_status, 100);
  else
    console.log("check_loading_status() Stop.");

}

//使用群組
function load_usergroup()
{
  gl_usergroups = [];
  var targetURL = gl_target_server + "/php/vilsnodes.php";
  var p = $.getJSON(targetURL, 'loadusergroup=1&project_id='+project_id, function(data)
  {
      $.each(data, function(index, group)
      {
          group.groupid = parseInt(group.groupid);
          gl_usergroups.push(group);
      });
  })
  .success(function() {
    CurrentLoadIndex = CurrentLoadIndex + 1;
  })
  .error(function() {
  })
  .complete(function() {
  });
}
//子集合
function load_userdatasubgroup()
{
  gl_userdatasubgroups = [];

  var targetURL = gl_target_server + '/php/vilsnodes.php';
  var p = $.getJSON(targetURL, 'loaddatasubgroup=1&project_id='+project_id+'&type=0', function(data)
  {
      $.each(data, function(index, group)
      {
          group.subgroupid = parseInt(group.subgroupid);
          gl_userdatasubgroups.push(group);
      });
  })
  .success(() => {
    CurrentLoadIndex = CurrentLoadIndex + 1;
  })
  .error(() => {
  })
  .complete(() => {
  });
}

//載入其他事件資料
function load_otherEvnet_records()
{

  var o = (gl_other_CurrentPage - 1) * LOAD_PAGE_NUM;
  var count = LOAD_PAGE_NUM;
  var targetURL = gl_target_server + '/php/eventreport.php';
  gl_other_event = [];
  var p = $.getJSON(targetURL, `loadotherevent=1&offset=${o}&count=${count}&project_id=${project_id}`, function(data)
  {
      $.each(data, (index, ev) => {
        gl_other_event.push(ev);
      });
  })
  .success(() => {
      show_eventreport_other_records();
      create_chart(tabKey.tab_other);

      var t = [];
      // t.push({'y'})


      gl_other_line_chart = new Morris.Line({
          element             : `${tabKey.tab_other}_chart_line_chart`,   //要放置圖表的區塊 ID、DOM 或是物件
          resize              : true,         //是否自動隨外部容器大小調整時同步調整大小
          redraw              : true,         //
          data                : gl_other_event,                 //要繪製成圖表的資料來源，需要一個物件陣列(物件的屬性並沒有特別的限制，只要把要顯示的 x 及 y 軸設定在 xkey 及 ykey 中就可以。不過 x 基本上是要是時間格式)
          xkey                : 'datetime',          //對應 data 物件陣列中物件的 x 軸(時間可以允許有 2012, 2012 Q1, 2012 W1, 2012-02, 2012-02-24, 2012-02-24 15:00, 2012-02-24 15:00:00, 2012-02-24 15:00:00.000 等格式，建議查看 dateFormat 參數)
          ykeys               : ['id','PROJECT_ID'],    //對應 data 物件陣列中物件的 y 軸(不管要對應幾個都要用陣列的方式來設定)
          labels              : ['id','PROJECT_ID'],       //對應 ykeys 的描述名稱
          lineColors          : ['#227744','#c8c8c8'],    //線及點的顏色陣列
          parseTime           : 2,            //預設會依 data 的 x 軸值來產生相對應時間的間距；若設為 false 則把每個時間點都設成一樣的間距
          hideHover           : 'auto',       //預設滑鼠移出圖表時提示框不會隱藏；若設為 true 則當滑鼠移出圖表就立即隱藏
          pointSize           : 4,            //點(資料結點)的直徑；單位 px
          pointStrokeColors   : '#227744',    //點(資料結點)的外框顏色；預設#FFFFFF
          gridTextColor       : '#222',       //圖表 x 及 y 軸的上的文字顏色
          gridStrokeWidth     : 0.4,          //圖表 y 軸要切成幾等份的線的寬度；單位 px
          gridTextFamily      : 'Open Sans',  //圖表 x 及 y 軸的上的文字字體；預設sans-serif
          gridTextSize        : 10,           //圖表 x 及 y 軸的上的文字大小；單位 px
      });

      // gl_other_line_chart = create_line_chart(tabKey.tab_other, '測試', '#227744');
      // adjust_chart_max_min(gl_other_line_chart, gl_other_event, 0, 100, '');
  })
  .error(() => {})
  .complete(() => {});

}
//載入燈泡事件資料
function load_lightEvent_records()
{
    var o = (gl_light_CurrentPage -1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;
    var targetURL = gl_target_server + '/php/eventreport.php';
    gl_light_event = [];
    var p = $.getJSON(targetURL, `loadlightevent=1&offset=${o}&count=${count}&project_id=${project_id}`, function(data)
    {
        $.each(data, (index, ev) => {
            gl_light_event.push(ev);
        });
    })
    .success(() =>
    {
        show_eventreport_light_records();
        create_chart(tabKey.tab_light);
        gl_light_line_chart = create_line_chart(`${tabKey.tab_light}_chart`, ['亮度'], ['#227744'], ['state_level']);
        adjust_chart_max_min(gl_light_line_chart, gl_light_event, -50, 100, ['state_level']);
        gl_light_line_chart.redraw();
    })
    .error(() => {})
    .complete(() => {});
}
//載入微波事件資料
function load_uwEvent_records()
{
    var o = (gl_uw_CurrentPage -1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;
    var targetURL = gl_target_server + '/php/eventreport.php';
    gl_uw_event = [];
    var p = $.getJSON(targetURL, `loaduwevent=1&offset=${o}&count=${count}&project_id=${project_id}`, function(data)
    {
        $.each(data, (index, ev) => {
            gl_uw_event.push(ev);
        });
    })
    .success(() =>
    {
        show_eventreport_uw_records();
        create_chart(tabKey.tab_uw);
        gl_uw_line_chart = create_line_chart(`${tabKey.tab_uw}_chart`, ['Rssi'], ['#227744'], ['Rssi']);
        adjust_chart_max_min(gl_uw_line_chart, gl_uw_event, -100, 0, ['Rssi']);
    })
    .error(() => {})
    .complete(() => {});
}
//載入智慧手錶事件資料
function load_watchEvent_records()
{
    var o = (gl_watch_CurrentPage -1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;
    var targetURL = gl_target_server + '/php/eventreport.php';
    gl_watch_event = [];
    var p = $.getJSON(targetURL, `loadwatchevent=1&offset=${o}&count=${count}&project_id=${project_id}`, function(data)
    {
        $.each(data, (index, ev) => {
            gl_watch_event.push(ev);
        });
    })
    .success(() =>
    {
        show_eventreport_watch_records();
        create_chart(tabKey.tab_watch);
        gl_watch_line_chart = create_line_chart(`${tabKey.tab_watch}_chart`, ['舒張壓','收縮壓'], ['#227744','#880044'], ['DBP','SBP']);
        adjust_chart_max_min(gl_watch_line_chart, gl_watch_event, -50, 100, ['DBP', 'SBP']);

    })
    .error((err) => {
        console.log(err);
    })
    .complete(() => {});

}
//載入智慧電錶事件資料
function load_pmEvent_records()
{
    var o = (gl_pm_CurrentPage -1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;
    var targetURL = gl_target_server + '/php/eventreport.php';
    gl_pm_event = [];
    var p = $.getJSON(targetURL, `loadpmevent=1&offset=${o}&count=${count}&project_id=${project_id}`, function(data)
    {
        $.each(data, (index, ev) => {
            gl_pm_event.push(ev);
        });
    })
    .success(() =>
    {
        show_eventreport_pm_records();

        //電壓
        create_chart(tabKey.tab_pm, `${tabKey.tab_pm}_v_chart`);
        gl_pm_line_chart_v = create_line_chart(`${tabKey.tab_pm}_v_chart`, ['電壓'], ['#227744'], ['Voltage_RMS']);
        adjust_chart_max_min(gl_pm_line_chart_v, gl_pm_event, 100, 130, ['Voltage_RMS']);
        gl_pm_line_chart_v.redraw();

        //功耗
        create_chart(tabKey.tab_pm, `${tabKey.tab_pm}_w_chart`);
        gl_pm_line_chart_w = create_line_chart(`${tabKey.tab_pm}_w_chart`, ['功耗'], ['#227744'], ['Active_Energy']);
        adjust_chart_max_min(gl_pm_line_chart_w, gl_pm_event, 500, 600, ['Active_Energy']);
        gl_pm_line_chart_w.redraw();

    })
    .error((err) => {
        console.log(err);
    })
    .complete(() => {});
}

//繪製其他事件表格
function show_eventreport_other_records() {
  var objTbody = $('#table_other')[0];
  var objTr;
  var objTd;

  //清除前次列表
  $('#table_other tr').remove();
  //新增本次資料列
  $.each(gl_other_event, (index, record) => {

    var nodeid = JSON.parse(record['nodeid']);

    objTr = document.createElement('tr');
    $(objTbody).append(objTr);

    //序(id)
    objTd = document.createElement('td');
    $(objTd).html(record['id']);
    $(objTr).append(objTd);

    //專案代碼(project_id)
    objTd = document.createElement('td');
    $(objTd).html(record['PROJECT_ID']);
    $(objTr).append(objTd);

    //事件編號(eventid)
    objTd = document.createElement('td');
    $(objTd).html(record['eventid']);
    $(objTr).append(objTd);

    //節點編號(nodeid)
    objTd = document.createElement('td');
    $(objTd).html(nodeid['nodeid']);
    $(objTr).append(objTd);

    //動作(action)
    objTd = document.createElement('td');
    $(objTd).attr('data-action', nodeid['action']);
    $(objTd).html(convert_action_name(nodeid['action']));
    $(objTr).append(objTd);

    //發生時間(datetime)
    objTd = document.createElement('td');
    objSpan = document.createElement("span");
    $(objSpan).attr("class", "label label-info");
    $(objSpan).html(record['datetime']);
    $(objTd).append(objSpan);
    $(objTr).append(objTd);



  });

}
//繪製燈泡事件表格
function show_eventreport_light_records()
{
    var objTbody = $('#table_light')[0];
    var objTr;
    var objTd;

    //清除前次列表
    $('#table_light tr').remove();
    //新增本次資料列
    $.each(gl_light_event, (index, record) => {
        objTr = document.createElement('tr');
        $(objTbody).append(objTr);

        objTd = document.createElement('td');
        $(objTd).html(record['id']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['name']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['uniAddress']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['state_onOff']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['state_level']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['state_cct']);
        $(objTr).append(objTd);

        //發生時間(datetime)
        objTd = document.createElement('td');
        objSpan = document.createElement("span");
        $(objSpan).attr("class", "label label-info");
        $(objSpan).html(record['time']);
        $(objTd).append(objSpan);
        $(objTr).append(objTd);
    });
}
//繪製微波事件表格
function show_eventreport_uw_records()
{
    var objTbody = $('#table_uw')[0];
    var objTr;
    var objTd;

    //清除前次列表
    $('#table_uw tr').remove();
    //新增本次資料列
    $.each(gl_uw_event, (index, record) => {
        objTr = document.createElement('tr');
        $(objTbody).append(objTr);

        objTd = document.createElement('td');
        $(objTd).html(record['Device_ID']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['Rssi']);
        $(objTr).append(objTd);

        //發生時間(datetime)
        objTd = document.createElement('td');
        objSpan = document.createElement("span");
        $(objSpan).attr("class", "label label-info");
        $(objSpan).html(record['time']);
        $(objTd).append(objSpan);
        $(objTr).append(objTd);
    });
}
//繪製智慧手錶事件表格
function show_eventreport_watch_records()
{
    var objTbody = $('#table_watch')[0];
    var objTr;
    var objTd;

    //清除前次列表
    $('#table_watch tr').remove();
    //新增本次資料列
    $.each(gl_watch_event, (index, record) => {
        objTr = document.createElement('tr');
        $(objTbody).append(objTr);

        objTd = document.createElement('td');
        $(objTd).html(record['Device_ID']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['Blood_Oxygen']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['DBP']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['SBP']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['Heart_Rate']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['Rssi']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['SOS']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['Temperature']);
        $(objTr).append(objTd);

        //發生時間(datetime)
        objTd = document.createElement('td');
        objSpan = document.createElement("span");
        $(objSpan).attr("class", "label label-info");
        $(objSpan).html(record['time']);
        $(objTd).append(objSpan);
        $(objTr).append(objTd);
    });
}
//繪製智慧電錶事件表格
function show_eventreport_pm_records()
{
    var objTbody = $('#table_pm')[0];
    var objTr;
    var objTd;

    //清除前次列表
    $('#table_pm tr').remove();

    //新增本次資料列
    $.each(gl_pm_event, (index, record) => {
        objTr = document.createElement('tr');
        $(objTbody).append(objTr);

        objTd = document.createElement('td');
        $(objTd).html(record['Device_ID']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['Active_Energy']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['Active_Power']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['BT_version']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['Current_RMS']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['Frequency']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['Rssi']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['Voltage_RMS']);
        $(objTr).append(objTd);

        objTd = document.createElement('td');
        $(objTd).html(record['relay_state']);
        $(objTr).append(objTd);

        //發生時間(datetime)
        objTd = document.createElement('td');
        objSpan = document.createElement("span");
        $(objSpan).attr("class", "label label-info");
        $(objSpan).html(record['time']);
        $(objTd).append(objSpan);
        $(objTr).append(objTd);
    });
}

//前一頁
function move_page_backward(tabName)
{
    var curPageNumUi = document.getElementById(`id_${tabName}_current_page_num`);
    var curPage = -1;

    switch (tabName) {
        case tabKey.tab_other:
            if (gl_other_CurrentPage > 1)
            {
                gl_other_CurrentPage--;
                curPage = gl_other_CurrentPage;
            }
            break;
        case tabKey.tab_light:
            if (gl_light_CurrentPage > 1)
            {
                gl_light_CurrentPage--;
                curPage = gl_light_CurrentPage;
            }
            break;
        case tabKey.tab_uw:
            if (gl_uw_CurrentPage > 1)
            {
                gl_uw_CurrentPage--;
                curPage = gl_uw_CurrentPage;
            }
            break;
        case tabKey.tab_watch:
            if (gl_watch_CurrentPage > 1)
            {
                gl_watch_CurrentPage--;
                curPage = gl_watch_CurrentPage;
            }
            break;
        case tabKey.tab_pm:
            if (gl_pm_CurrentPage > 1)
            {
                gl_pm_CurrentPage--;
                curPage = gl_pm_CurrentPage;
            }
            break;
    }

    if (curPage != -1)
    {
        curPageNumUi.textContent = curPage;
        reload_page();
    }

}
//後一頁
function move_page_forward(tabName)
{
    var curPageNumUi = document.getElementById(`id_${tabName}_current_page_num`);
    var curPage = -1;
    switch (tabName) {
        case tabKey.tab_other:
            gl_other_CurrentPage++;
            curPage = gl_other_CurrentPage;
            break;
        case tabKey.tab_light:
            gl_light_CurrentPage++;
            curPage = gl_light_CurrentPage;
            break;
        case tabKey.tab_uw:
            gl_uw_CurrentPage++;
            curPage = gl_uw_CurrentPage;
            break;
        case tabKey.tab_watch:
            gl_watch_CurrentPage++;
            curPage = gl_watch_CurrentPage;
            break;
        case tabKey.tab_pm:
            gl_pm_CurrentPage++;
            curPage = gl_pm_CurrentPage;
            break;
    }
    curPageNumUi.textContent = curPage;
    reload_page();
}

//開啟事件彙總表頁面
function open_eventreport_page()
{
    var win = window.open('/index.php/vilseventreport/'+project_id, '_blank');
    win.focus();
}
//重新載入
function reload_page()
{
    PreLoadIndex = 0;
    CurrentLoadIndex = 2;
    check_loading_status();
}
//切換頁籤
function scott_tab_onclick(tabName)
{
    console.log(`scott_tab_onclick() tabname:${tabName}`);
    gl_activetab = tabName;
    switch (tabName) {
        case tabKey.tab_other:
            gl_other_line_chart.redraw();
            break;
        case tabKey.tab_light:
            gl_light_line_chart.redraw();
            break;
        case tabKey.tab_uw:
            gl_uw_line_chart.redraw();
            break;
        case tabKey.tab_watch:
            gl_watch_line_chart.redraw();
            break;
        case tabKey.tab_pm:
            gl_pm_line_chart_v.redraw();
            gl_pm_line_chart_w.redraw();
            break;
    }
}

//繪製其他事件圖表
function create_chart(tabName, subChart)
{
    var areaColor = '';
    var areaTitle = '';
    var chartId = '';
    var chartBlock;

    switch (tabName) {
        case tabKey.tab_other:
            areaColor = 'bg-teal-gradient';
            areaTitle = '其他事件監看';
            break;
        case tabKey.tab_light:
            areaColor = 'bg-yellow-gradient';
            areaTitle = '燈泡監看(亮度)';
            break;
        case tabKey.tab_uw:
            areaColor = 'bg-red-gradient';
            areaTitle = '微波監看';
            break;
        case tabKey.tab_watch:
            areaColor = 'bg-green-gradient';
            areaTitle = '手錶監看';
            break;
        case tabKey.tab_pm:
            areaColor = 'bg-aqua-gradient';
            switch (subChart) {
                case `${tabKey.tab_pm}_v_chart`:
                    areaTitle = '電錶監看[電壓]';
                    break;
                case `${tabKey.tab_pm}_w_chart`:
                    areaTitle = '電錶監看[功耗]';
                    break;
            }
            break;
    }

    if (subChart != undefined && subChart != '')
        chartId = subChart;
    else
        chartId = `${tabName}_chart`;


    chartBlock = create_chart_block(chartId, areaColor, areaTitle);
    $(`#${chartId}`).remove();
    $(`#div_${tabName}_chart_area`).append(chartBlock);
}
//繪製區塊圖表
function create_chart_block(chartId, areaColor, areaTitle)
{
    var chart_col = document.createElement('div');
    chart_col.setAttribute("class", "col-xs-12 col-sm-12 col-md-6 col-lg-6");
    chart_col.setAttribute("id", chartId);

    var chart_col_box = document .createElement('div');
    chart_col_box.setAttribute("class", `box box-solid ${areaColor}`);
    chart_col.appendChild(chart_col_box);

    var chart_col_box_header = document.createElement('div');
    chart_col_box_header.setAttribute("class", "box-header");
    chart_col_box.appendChild(chart_col_box_header);

    var chart_col_box_header_i = document.createElement('i');
    chart_col_box_header_i.setAttribute("class", "fa fa-th");
    chart_col_box_header.appendChild(chart_col_box_header_i);

    var chart_col_box_header_title = document.createElement('h3');
    chart_col_box_header_title.setAttribute("class", "box-title");
    chart_col_box_header_title.setAttribute("id", `${chartId}_title`);
    chart_col_box_header_title.textContent = areaTitle;
    chart_col_box_header.appendChild(chart_col_box_header_title);

    var chart_col_box_body = document.createElement("div");
    chart_col_box_body.setAttribute("class", "box-body border-radius-none");
    chart_col_box.appendChild(chart_col_box_body);

    var chart_col_box_body_chart = document.createElement('div');
    chart_col_box_body_chart.setAttribute("class", "chart");
    chart_col_box_body_chart.setAttribute("style", "height: 300px");
    chart_col_box_body_chart.setAttribute("id", `${chartId}_line_chart`);
    chart_col_box_body.appendChild(chart_col_box_body_chart);

    return chart_col;
}
//設定線形圖相關參數
function create_line_chart(tabName, show_label, lineColor, fds)
{
    var lineChart1 = new Morris.Line({
        element             : `${tabName}_line_chart`,   //要放置圖表的區塊 ID、DOM 或是物件
        resize              : true,         //是否自動隨外部容器大小調整時同步調整大小
        redraw              : true,         //
        xkey                : 'y',          //對應 data 物件陣列中物件的 x 軸(時間可以允許有 2012, 2012 Q1, 2012 W1, 2012-02, 2012-02-24, 2012-02-24 15:00, 2012-02-24 15:00:00, 2012-02-24 15:00:00.000 等格式，建議查看 dateFormat 參數)
        ykeys               : fds,        //對應 data 物件陣列中物件的 y 軸(不管要對應幾個都要用陣列的方式來設定)
        labels              : show_label, //對應 ykeys 的描述名稱
        lineColors          : lineColor,  //線及點的顏色陣列
        parseTime           : 2,            //預設會依 data 的 x 軸值來產生相對應時間的間距；若設為 false 則把每個時間點都設成一樣的間距
        hideHover           : 'auto',       //預設滑鼠移出圖表時提示框不會隱藏；若設為 true 則當滑鼠移出圖表就立即隱藏
        pointSize           : 4,            //點(資料結點)的直徑；單位 px
        pointStrokeColors   : lineColor,  //點(資料結點)的外框顏色；預設#FFFFFF
        gridTextColor       : '#222',       //圖表 x 及 y 軸的上的文字顏色
        gridStrokeWidth     : 0.4,          //圖表 y 軸要切成幾等份的線的寬度；單位 px
        gridTextFamily      : 'Open Sans',  //圖表 x 及 y 軸的上的文字字體；預設sans-serif
        gridTextSize        : 10,           //圖表 x 及 y 軸的上的文字大小；單位 px
    });
    return lineChart1;
}
//
function adjust_chart_max_min(uiLineChart, dataInfoList, dfMin, dfMax, fds)
{
    var dataMin = 200;
    var dataMax = 0;
    var dataItemList = [];

    for (var index = dataInfoList.length -1; index >= 0; index--)
    {
        var dataInfo = dataInfoList[index];
        var itemValue;
        var info = [];

        $.each(fds, (idx, fd) => {
            itemValue = parseFloat(dataInfo[fd]);
            info[fd] = itemValue.toFixed(2);
            if (dataMin > itemValue)
                dataMin = itemValue;
            if (dataMax < itemValue)
                dataMax = itemValue;
        });

        info['y'] = dataInfo.time;
        dataItemList.push(info);

    }

    if (dfMax < 0)
        uiLineChart.options['ymax'] = parseInt(dataMax) + 2;
    else
        uiLineChart.options['ymax'] = dfMax;

    if (dfMin < 0)
        uiLineChart.options['ymin'] = parseInt(dataMin);
    else
        uiLineChart.options['ymin'] = dfMin;

    uiLineChart.setData(dataItemList);
    uiLineChart.drawEvents();
}

var glStartDate = moment().subtract(6, 'days');
var glEndDate = moment();

$('#daterange-btn').daterangepicker(
  {
    ranges   : {
      '今天'       : [moment(), moment()],
      '昨天'   : [moment().subtract(1, 'days'), moment().subtract(1, 'days')],
      '最近7天' : [moment().subtract(6, 'days'), moment()],
      '最近30天': [moment().subtract(29, 'days'), moment()],
      '這個月'  : [moment().startOf('month'), moment().endOf('month')],
      '上個月'  : [moment().subtract(1, 'month').startOf('month'), moment().subtract(1, 'month').endOf('month')]
    },
    startDate: glStartDate,
    endDate  : glEndDate,
    opens: 'right',
    autoApply: true,
    // locale: { cancelLabel: 'Cancel', applyLabel: '應用' }
  },
  function (start, end) {
    $('#daterange-btn span').html(start.format('YYYY/MM/DD ') + ' - ' + end.format('YYYY/MM/DD'))
  }
)

$('#daterange-btn').on('apply.daterangepicker', function(ev, picker)
{
    glStartDate = picker.startDate;
    glEndDate = picker.endDate;

    // // check is tag or user
    // if (glCurrentTag_nodeid.length > 0)
    // {
    //     load_tag_bodyinfo_records(1);
    // }
    // else
    // if (glCurrentUser_accountid.length > 0)
    // {
    //     load_user_bodyinfo_records(1);
    // }

    // console.log(picker.startDate.format('YYYY/MM/DD'));
    // console.log(picker.endDate.format('YYYY/MM/DD'));
});
