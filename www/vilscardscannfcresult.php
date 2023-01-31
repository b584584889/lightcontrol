<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">
    <link href="/css/vils.min.css" rel="stylesheet">
    <meta name="description" content="SMIMS VILS IoT">
    <meta name="author" content="SMIMS">
    <title>SMIMS VILS IoT - NFC CARD Scan Result</title>
  </head>

  <div class="row">
      <div class="col-md-12">
          <div class="box">
            <div class="box-header with-border">
              <h3 class="box-title"><?php echo $vilscardscannfcresult_title;?></h3>
              </button>
              <div class="btn-group">
                  <input type="text" id="search_nfccard_text" placeholder="尋找卡片ID">
              </div>
              <div class="box-tools pull-right">
                  <ul class="pagination pagination-sm no-margin">
                    <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                    <li><a href="#" id="id_current_page_num">1</a></li>
                    <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
                  </ul>
                  <ul class="pagination pagination-sm no-margin pull-right">
                    <li>
                      <button type="button" class="btn btn-box-tool" onclick="open_cardscannfcresult_page();return false;"><i class="fa fa-external-link"></i>
                      </button>
                  </li>
                </ul>
              </div>
            </div>
            <!-- /.box-header -->
            <div class="box-body table-responsive">

                <!-- orig -->
                <!-- <table class="table table-bordered" id="RANGING_TABLE">
                </table> -->
                <div id="CARDSCAN_TABLE_MSG"><?php echo $vilscardscannfcresult_loading;?></div>

                <table class="table table-bordered">
                  <thead>
                    <tr style="background-color:#cce6ff;">
                      <th style="width: 10px">#</th>
                      <th><?php echo $vilscardscannfcresult_cardname;?></th>
                      <th><?php echo $vilscardscannfcresult_cardid;?></th>
                      <th><?php echo $vilscardscannfcresult_nfcdevice;?></th>
                      <th><?php echo $vilscardscannfcresult_user;?></th>
                      <th><?php echo $vilscardscannfcresult_updatetime;?></th>
                      <th><?php echo $vilscardscannfcresult_ssid;?></th>
                      <th><?php echo $vilscardscannfcresult_wifimac;?></th>
                    </tr>
                  </thead>
                  <tbody id="CARDSCAN_TABLE">
                  </tbody>
                </table>

             </div>
             <!-- /.box-body -->
          </div>
          <!-- /.box -->
      </div>
      <!--/.col (left) -->
  </div>
  <!--/.row -->


  <script>
    var project_id = <?php echo $project_id ?>;
    var accountid = "<?php echo $_SESSION["accountid"] ?>";
    var accountname = "<?php echo $_SESSION["accountname"] ?>";
    var groupid = "<?php echo $_SESSION["groupid"] ?>";
  </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/cardscannfcresult.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/cardscannfcresult_src/00-cardscannfcresult.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</html>
