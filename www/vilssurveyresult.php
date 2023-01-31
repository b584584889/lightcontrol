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
    <meta name="description" content="SMIMS VILS IoT">
    <meta name="author" content="SMIMS">
    <title>SMIMS VILS IoT - Survey Result</title>
  </head>

      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $vilssurveyrecord_title;?></h3>
          <div class="box-tools pull-right">
              <ul class="pagination pagination-sm no-margin">
                <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                <li><a href="#" id="id_current_page_num">1</a></li>
                <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
              </ul>
              <ul class="pagination pagination-sm no-margin pull-right">
                <li>
                  <button type="button" class="btn btn-box-tool" onclick="open_surveyrecords_page();return false;"><i class="fa fa-external-link"></i>
                  </button>
                </li>
              </ul>
          </div>
        </div>
        <!-- /.box-header -->
        <div class="box-body">

          <table class="table table-bordered" id="SURVEY_EVENT_TABLE">
            <thead>
              <tr style="background-color:rgb(236, 240, 245);">
                <th style="width: 30px">#</th>
                <th>紀錄id</th>
                <th>事項名稱</th>
                <th>事項說明</th>
                <th style="width: 60px">類型</th>
                <!-- <th>事項列表</th> -->
                <th>回報截止時間</th>
                <th>推播時間</th>
                <th>推播照片</th>
                <!-- <th>操作</th> -->
              </tr>
            </thead>
            <tbody id="SURVEY_EVENT_TABLE_BODY">

            </tbody>
          </table>

          <table class="table table-bordered">
            <thead>
              <tr style="background-color:rgb(236, 240, 245);">
                <th>已讀用戶</th>
              </tr>
            </thead>
            <tbody>
                <tr> <td><div id="message_read"></div></td>
                </tr>
            </tbody>
          </table>

          <table class="table table-bordered" id="SURVEY_ITEM_TABLE">
            <thead>
              <tr style="background-color:rgb(236, 240, 245);">
                <th style="width: 30px">#</th>
                <th>事項列表名稱</th>
                <th>用戶回傳結果</th>
                <th>回傳用戶</th>
              </tr>
            </thead>
            <tbody id="SURVEY_ITEM_TABLE_BODY">

            </tbody>
          </table>

          <table class="table table-bordered" id="UPLOAD_IMAGE_TABLE">
            <thead>
              <tr style="background-color:rgb(236, 240, 245);">
                <th style="width: 30px">#</th>
                <th>上傳照片</th>
                <th>回傳用戶</th>
              </tr>
            </thead>
            <tbody id="UPLOAD_IMAGE_TABLE_BODY">
            </tbody>
          </table>


        </div>
        <!-- /.box-body -->
      </div>
      <!-- /.box -->


    <script>
        var project_id = <?php echo $project_id ?>;
        var accountid = "<?php echo $_SESSION["accountid"] ?>";
        var accountname = "<?php echo $_SESSION["accountname"] ?>";
        var groupid = "<?php echo $_SESSION["groupid"] ?>";

        var show_hashkey = "<?php echo $hashkey ?>";
        var show_surveyid = "<?php echo $surveyid ?>";

    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/surveyresult.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/surveyresult_src/00-surveyresult.js"></script> -->
</html>
