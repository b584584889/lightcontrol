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
    <title>SMIMS VILS IoT - Push Notification Records</title>
  </head>

      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $vilspushnotification_records;?></h3>

            <span>&nbsp;&nbsp;</span>
            <div class="btn-group">
              <button type="button" class="btn btn-default" id="current_type_title"></button>
              <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
                <span class="caret"></span>
                <span class="sr-only">Toggle Dropdown</span>
              </button>
              <ul class="dropdown-menu" id="dropdown_menu_type" role="menu">
              </ul>
            </div>

            <span>&nbsp;&nbsp;</span>
            <div class="btn-group">
              <button type="button" class="btn btn-default" id="current_theme_title"></button>
              <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
                <span class="caret"></span>
                <span class="sr-only">Toggle Dropdown</span>
              </button>
              <ul class="dropdown-menu" id="dropdown_menu_theme" role="menu">
              </ul>
            </div>


          <div class="box-tools pull-right">
              <ul class="pagination pagination-sm no-margin">
                <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                <li><a href="#" id="id_current_page_num">1</a></li>
                <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
              </ul>
              <ul class="pagination pagination-sm no-margin pull-right">
                <li>
                  <button type="button" class="btn btn-box-tool" onclick="open_pushnotificationrecords_page();return false;"><i class="fa fa-external-link"></i>
                  </button>
                </li>
              </ul>
          </div>

          <div class="btn-group">
              <input type="text" id="search_notify_target_text" placeholder="尋找推播對象">
          </div>
          <div class="btn-group">
              <input type="text" id="search_notify_date_text" placeholder="日期:YYYY-MM-DD">
          </div>

        </div>
        <!-- /.box-header -->
        <div class="box-body">

            <div style="height:650px;overflow:scroll;">
                <table class="table table-bordered">
                    <thead>
                        <tr style="background-color:rgb(236, 240, 245);">
                            <th style="width: 10px">#</th>
                            <th style="width: 130px">推播類型</th>
                            <th style="width: 80px">觸發裝置</th>
                            <th style="width: 80px">推播對象</th>
                            <th style="width: 80px">已讀取</th>
                            <th>Title</th>
                            <th>訊息內容</th>
                            <th style="width: 100px"時間</th>
                            <th style="width: 50px">操作</th>
                        </tr>
                    </thead>
                    <tbody id="table_content">
                    </tbody>
                </table>
            </div>


          <div class="nav-tabs-custom">
            <ul class="nav nav-tabs" id="nav_tabs">
            </ul>
            <div class="tab-content" id="tab_content">
            </div>
          </div>

        </div>
        <!-- /.box-body -->
      </div>
      <!-- /.box -->


    <script>
        var project_id = <?php echo $project_id ?>;
        var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
        var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
        var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/pushnotificationrecords.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/pushnotificationrecords_src/00-pushnotificationrecords.js"></script> -->
</html>
