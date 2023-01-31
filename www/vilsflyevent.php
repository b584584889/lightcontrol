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
    <title>SMIMS VILS IoT - Fly Event</title>
  </head>

      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title">航班資訊</h3>

          <div class="box-tools pull-right">
              <ul class="pagination pagination-sm no-margin">
                <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                <li><a href="#" id="id_current_page_num">1</a></li>
                <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
              </ul>
              <ul class="pagination pagination-sm no-margin pull-right">
                <li>
                  <button type="button" class="btn btn-box-tool" onclick="open_flyevent_page();return false;"><i class="fa fa-external-link"></i>
                  </button>
                </li>
              </ul>
          </div>

          <input type="checkbox" id="ArrivalCheckBtn" onchange="toggleArrivalCheckbox(this)" checked>抵達</input>
          <input type="checkbox" id="DepartureCheckBtn" onchange="toggleDepartureCheckbox(this)">起飛</input>

          <div class="btn-group">
              <input type="text" id="search_sensorid_text" placeholder="尋找傳感器">
          </div>

        </div>
        <!-- /.box-header -->
        <div class="box-body">

            <div style="height:650px;overflow:scroll;">
                <table class="table table-bordered">
                    <thead>
                        <tr style="background-color:rgb(236, 240, 245);">
                            <th style="width: 10px">#</th>
                            <th>eventid</th>
                            <th>sensorid</th>
                            <th>抵達/起飛</th>
                            <th>日期</th>
                            <th>表定時間</th>
                            <th>預計時間</th>
                            <th>機坪時間</th>
                            <th>實際時間</th>
                            <th>航空公司</th>
                            <th>班機</th>
                            <th>機場</th>
                            <th>班機型號</th>
                            <th>偵測時間</th>
                            <th>紀錄時間</th>
                            <th style="width: 50px">操作</th>
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
    <!-- <script src="/js/flyevent.min.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

    <script src="/js/flyevent_src/00-flyevent.js?date=<?php echo $UPDATE_DATE;?>"></script>

</html>
