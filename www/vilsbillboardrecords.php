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
    <title>SMIMS VILS IoT - Billboard Records</title>
  </head>

      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $vilsbillboardrecords_title;?></h3>

          <div class="box-tools pull-right">
              <ul class="pagination pagination-sm no-margin">
                <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                <li><a href="#" id="id_current_page_num">1</a></li>
                <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
              </ul>
              <ul class="pagination pagination-sm no-margin pull-right">
                <li>
                  <button type="button" class="btn btn-box-tool" onclick="open_billboardrecords_page();return false;"><i class="fa fa-external-link"></i>
                  </button>
                </li>
              </ul>
          </div>

          <div class="btn-group">
              <input type="text" id="search_billboard_title_text" placeholder="尋找公告標題">
          </div>
          <div class="btn-group">
              <input type="text" id="search_billboard_date_text" placeholder="日期:YYYY-MM-DD">
          </div>

        </div>
        <!-- /.box-header -->
        <div class="box-body">

            <div style="height:650px;overflow:scroll;">
                <table class="table table-bordered">
                    <thead>
                        <tr style="background-color:rgb(236, 240, 245);">
                            <th style="width: 10px">#</th>
                            <th style="width: 180px">標題</th>
                            <th>內容</th>
                            <th style="width: 80px">公告對象</th>
                            <th style="width: 100px">公告時間</th>
                            <th style="width: 50px">刪除</th>
                        </tr>
                    </thead>
                    <tbody id="table_content">
                    </tbody>
                </table>
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
    <script src="/js/billboardrecords_src/00-billboardrecords.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <!-- <script src="/js/billboardrecords.min.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

    <!-- <script src="/js/billboardrecords_src/00-billboardrecords.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</html>
