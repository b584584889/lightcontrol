var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var gl_pm_list = [];
var gl_pm_event = [];
var gl_pm_line_chart_c = null;
var gl_pm_line_chart_v = null;
var gl_pm_line_chart_w = null;
var gl_pm_line_chart_p = null;

var gl_runtime = true;
var gl_firstlload = true;

// 七天開始日
var glStartDate = moment().subtract(1, 'days'); // moment().subtract(6, 'days');
// 七天截止日
var glEndDate = moment();

const chartKey =
{
    chart_pm_c: 'pm_c',   //電流[Current_RMS]
    chart_pm_v: 'pm_v',   //電壓[Voltage_RMS]
    chart_pm_w: 'pm_w',   //功耗[Active_Energy]
    chart_pm_p: 'pm_p'    //功率[Active_Power]
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
    case 2: //載入PM清單
      if (PreLoadIndex != CurrentLoadIndex)
      {
        console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
        PreLoadIndex = CurrentLoadIndex;

        load_pm_list();
        CurrentLoadIndex++;
      }
      break;
    case 3: //載入資料
      if (PreLoadIndex != CurrentLoadIndex)
      {
        console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
        PreLoadIndex = CurrentLoadIndex;

        load_pmEvent_records();
        CurrentLoadIndex++;
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
// 取得ＰowerMeter ID 清單
function load_pm_list()
{
    gl_pm_list = [];
    var targetUrl = gl_target_server + "/php/powermeterview.php";
    var p = $.getJSON(targetUrl, `loadsensorid=1&project_id=${project_id}`, function (data)
    {
        $.each(data, (index, ev) => {
            gl_pm_list.push(ev);
        })
    })
    .success(() => {
        create_pm_list();
    })
    .error(err => {
        console.log(`powermeterview -> load_pm_list: ${err} `);
    })
    .complete(() => {

    });
}
//載入智慧電錶事件資料
function load_pmEvent_records()
{

    var dteStart = glStartDate.format("YYYY/MM/DD HH:mm:ss");
    var dteEnd = glEndDate.format("YYYY/MM/DD HH:mm:ss");
    var strId = $('.listItemSelected')[0].dataset['key'];

    // glStartDate = moment().subtract(1, 'days');
    // glEndDate = moment();
    gl_pm_event = [];

    var targetUrl = gl_target_server + "/php/powermeterview.php";
    var p = $.getJSON(targetUrl, `loadpmrecord=1&id=${strId}&sdate=${dteStart}&edate=${dteEnd}&project_id=${project_id}`, function(data)
    {
        $.each(data, (index, ev) => {
            gl_pm_event.push(ev);
        })
    })
    .success(() => {

        //電流
        create_chart(chartKey.chart_pm_c);
        gl_pm_line_chart_c = create_line_chart(chartKey.chart_pm_c, ['電流'], ['#227744'], ['Current_RMS']);
        adjust_chart_max_min(gl_pm_line_chart_c, gl_pm_event, -1, 1, ['Current_RMS']);
        //電壓
        create_chart(chartKey.chart_pm_v);
        gl_pm_line_chart_v = create_line_chart(chartKey.chart_pm_v, ['電壓'], ['#227744'], ['Voltage_RMS']);
        adjust_chart_max_min(gl_pm_line_chart_v, gl_pm_event, 100, 130, ['Voltage_RMS']);
        //功耗
        create_chart(chartKey.chart_pm_w);
        gl_pm_line_chart_w = create_line_chart(chartKey.chart_pm_w, ['功耗'], ['#227744'], ['Active_Energy']);
        adjust_chart_max_min(gl_pm_line_chart_w, gl_pm_event, 500, 700, ['Active_Energy']);
        //功率
        create_chart(chartKey.chart_pm_p);
        gl_pm_line_chart_p = create_line_chart(chartKey.chart_pm_p, ['功率'], ['#227744'], ['Active_Power']);
        adjust_chart_max_min(gl_pm_line_chart_p, gl_pm_event, 0, 10, ['Active_Power']);

        if (gl_runtime)
            setTimeout(load_pmEvent_records, 1000);

    })
    .error(err => {
        console.log(`powermeterview -> load_pmEvent_records: ${err} `);
    })
    .complete(() => {});
}

// 繪製PowerMeter清單表
function create_pm_list()
{
    // var objTbody = $('#pm_list_content');
    var idx = 0;
    $.each(gl_pm_list, (index, data) => {
        idx = index + 1;
        $('#pm_list_content').append(create_pm_list_item(idx, data));
        gl_firstlload = false;
    })
}
// 載入PowerMeter項目
function create_pm_list_item(index, data)
{
    var tr, td, span;

    tr = document.createElement('tr');

    td = document.createElement("td");
    td.setAttribute("class", "listItem listIndex" + (gl_firstlload ? " listItemSelected " : ""));
    td.setAttribute("data-field", "index");
    td.setAttribute("data-key", data['id']);
    td.textContent = index;
    tr.appendChild(td);

    td = document.createElement("td");
    td.setAttribute("class", "listItem listData" + (gl_firstlload ? " listItemSelected " : ""));
    td.setAttribute("data-field", "data");
    td.setAttribute("data-key", data['id']);
    tr.appendChild(td);

    span = document.createElement("span");
    span.setAttribute("class", "badge bg-aqua-active");
    span.textContent = data['id'];
    td.appendChild(span);

    return tr;
}
// 繪製其他事件圖表
function create_chart(chartName)
{
    var areaColor = '';
    var areaTitle = `${$('.listItemSelected')[0].dataset['key']}`;
    var chartId = '';
    var chartBlock;

    switch (chartName) {
        case chartKey.chart_pm_c:
            areaColor = 'bg-green-gradient';
            areaTitle += '電錶監看[電流]';
            break;
        case chartKey.chart_pm_v:
            areaColor = 'bg-teal-gradient';
            areaTitle += '電錶監看[電壓]';
            break;
        case chartKey.chart_pm_p:
            areaColor = 'bg-aqua-gradient';
            areaTitle += '電錶監看[功率]';
            break;
        case chartKey.chart_pm_w:
            areaColor = 'bg-red-gradient';
            areaTitle += '電錶監看[功耗]';
            break;
    }

    chartId = `${chartName}_chart`
    chartBlock = create_chart_block(chartId, areaColor, areaTitle);
    $(`#${chartId}`).remove();
    $(`#div_pm_chart_area`).append(chartBlock);
}
// 繪製區塊圖表
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
// 設定線形圖相關參數
function create_line_chart(tabName, show_label, lineColor, fds)
{
    var lineChart1 = new Morris.Line({
        element             : `${tabName}_chart_line_chart`,   //要放置圖表的區塊 ID、DOM 或是物件
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
// 更新圖表資料內容
function adjust_chart_max_min(uiLineChart, dataInfoList, dfMin, dfMax, fds)
{
    var dataMin;
    var dataMax;
    var dataItemList = [];
    for (var index = dataInfoList.length -1; index >= 0; index--)
    {
        var dataInfo = dataInfoList[index];
        var itemValue;
        var info = [];

        $.each(fds, (idx, fd) => {
            itemValue = parseFloat(dataInfo[fd]);
            info[fd] = itemValue.toFixed(2);
            if (dataMin == undefined || dataMin > itemValue)
                dataMin = itemValue;
            if (dataMax == undefined || dataMax < itemValue)
                dataMax = itemValue;
        });

        info['y'] = dataInfo.time;
        dataItemList.push(info);

    }
    uiLineChart.options['ymax'] = parseInt(dataMax) + 2;
    uiLineChart.options['ymin'] = parseInt(dataMin) - 2;
    uiLineChart.setData(dataItemList);
}
//
function pm_tab_search_text_change(e)
{
    var strValue = $('#pm_tab_search_text').val();

    $('#')

    var filterDatas = $.grep(gl_pm_list, (n, i) => {
        return n.id ;
    });


}

$(document).ready(function() {
    $('#pm_list_content').on('click', '.listItem', (ev) => {
        $('.listItem').removeClass('listItemSelected');
        var t = ev.currentTarget.dataset['key'];
        $(`.listItem[data-key=${ev.currentTarget.dataset['key']}]`).addClass("listItemSelected");
    });

    $('#pm_tab_search_text').on('input', ev => {
        pm_tab_search_text_change(ev);
    });
});

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

//開啟事件彙總表頁面
function open_powermeterview_page()
{
    var win = window.open('/index.php/vilspowermeterview/'+project_id, '_blank');
    win.focus();
}
