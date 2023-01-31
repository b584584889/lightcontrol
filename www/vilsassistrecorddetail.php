<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">
    <link rel="shortcut icon" sizes="16x16 32x32 48x48 64x64" href="/images/favicon.ico">
    <link href="/css/vils.min.css" rel="stylesheet">

    <style type="text/css">
    .datepicker table tr td.active:active,
    .datepicker table tr td.active:hover:active,
    .datepicker table tr td.active.disabled:active,
    .datepicker table tr td.active.disabled:hover:active,
    .datepicker table tr td.active.active,
    .datepicker table tr td.active:hover.active,
    .datepicker table tr td.active.disabled.active,
    .datepicker table tr td.active.disabled:hover.active
    {
      background-image: linear-gradient(to bottom, #cc4400, #994411);
    }
    /* .datepicker table tr td.highlight:active,
    .datepicker table tr td.highlight:hover:active,
    .datepicker table tr td.highlight.disabled:active,
    .datepicker table tr td.highlight.disabled:hover:active,
    .datepicker table tr td.highlight.active,
    .datepicker table tr td.highlight:hover.active,
    .datepicker table tr td.highlight.disabled.active,
    .datepicker table tr td.highlight.disabled:hover.active
    {
        background-image: linear-gradient(to bottom, #cc4400, #994411);
    } */
    .datepicker table tr td.new { display: none; }
    .datepicker table tr td.old { visibility: hidden; }
    .datepicker {
        background-color: #9fc49f !important;
        color: #333333 !important;
    }

    </style>

    <meta name="description" content="SMIMS VILS IoT">
    <meta name="author" content="SMIMS">
    <title>SMIMS VILS IoT - Assist Record Detail</title>
  </head>

      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $vilsassistrecord_title;?></h3>
        </div>
        <!-- /.box-header -->
        <div class="box-body table-responsive">


          <table class="table table-bordered" id="ASSIST_EVENT_TABLE">
            <thead>
              <tr style="background-color:rgb(236, 240, 245);">
                <th>申請時間</th>
                <th>事項名稱</th>
                <th>事項說明</th>
                <th>申請說明</th>
                <th>協助對象</th>
                <th>每日協助時間</th>
                <th style="width: 180px">協助日期</th>
                <th>附件照片</th>
                <th>附件檔案</th>
                <th>申請人姓名</th>
                <th>申請人關係</th>
                <th>狀態</th>
              </tr>
            </thead>
            <tbody id="ASSIST_EVENT_TABLE_BODY">

            </tbody>
          </table>

          <table class="table table-bordered">
              <thead>
              <tr style="background-color:#cce6ff;">
                <th style="width: 10px">#</th>
                <th>預約時間</th>
                <th>通知負責人時間</th>
                <th>負責人</th>
                <th>回報狀態</th>
                <th>回報訊息</th>
                <th>回報照片</th>
                <th>回報時間</th>
              </tr>
              </thead>
              <tbody id="ASSIST_EVENTRECORD_TABLE_BODY">
              </tbody>
          </table>

        </div>
        <!-- /.box-body -->
      </div>
      <!-- /.box -->

    <script>
        var project_id = <?php echo $project_id ?>;
        var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
        var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
        var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";

        var gl_assisteventid = "<?php echo $assisteventid ?>";
        var gl_assistid = "<?php echo $assistid ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/assistrecorddetail.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/assistrecorddetail_src/00-bootstrap-datepicker.js"></script>
    <script src="/js/assistrecorddetail_src/01-assistrecorddetail.js"></script> -->
</html>
