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
    <title>SMIMS VILS IoT - Curriculum Records</title>
  </head>

      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $vilscurriculumrecord_title;?></h3>

          <span>&nbsp;</span>
          <a class='btn btn-success btn-xs' onclick='create_new_curriculumrecord();'><?php echo $vilscurriculum_applynew; ?></a>

          <div class="box-tools pull-right">
              <ul class="pagination pagination-sm no-margin">
                <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                <li><a href="#" id="id_current_page_num">1</a></li>
                <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
              </ul>
              <ul class="pagination pagination-sm no-margin pull-right">
                <li>
                  <button type="button" class="btn btn-box-tool" onclick="open_curriculumrecords_page();return false;"><i class="fa fa-external-link"></i>
                  </button>
                </li>
              </ul>
          </div>
        </div>
        <!-- /.box-header -->
        <div class="box-body table-responsive">

          <table class="table table-bordered" id="CURRICULUM_RECORD_TABLE">
            <thead>
              <tr style="background-color:rgb(236, 240, 245);">
                <th style="width: 30px">#</th>
                <th style="width: 100px">發起人</th>
                <th style="width: 100px">編輯人員</th>
                <th style="width: 100px">課程類別[課程號碼]</th>
                <th style="width: 100px">課程名稱</th>
                <!-- <th>開放對象(主集合)</th> -->
                <th>開放選修日期</th>
                <th>課程日期</th>
                <th>課程時間</th>
                <th>課程週間日</th>
                <th>課程簽到時間</th>
                <th>總時數(小時)</th>
                <th>地點</th>
                <th>內容</th>
                <th>任課老師</th>
                <th>任課老師簡介</th>
                <th>預定人數</th>
                <th>自動核準</th>
                <th>費用</th>
                <th>備註</th>
                <th>簡章下載</th>
                <th>狀態</th>
                <th>操作</th>
              </tr>
            </thead>
            <tbody id="CURRICULUM_RECORD_TABLE_BODY">

            </tbody>
          </table>

        </div>
        <!-- /.box-body -->
      </div>
      <!-- /.box -->


    <script>
        var project_id = <?php echo $project_id ?>;
        var gl_userid = "<?php echo $_SESSION["userid"] ?>";
        var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
        var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
        var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/curriculumrecords.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/curriculumrecords_src/00-bootstrap-timepicker.js"></script>
    <script src="/js/curriculumrecords_src/01-bootstrap-datepicker.js"></script>
    <script src="/js/storagefile_src/02-uploadfile.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/storageimage_src/02-uploadimage.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/curriculumrecords_src/02-curriculumrecords.js"></script> -->
</html>
