<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
    <?php
    include ('vi_html_head.php');
    ?>

    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $cardrollcallrecord_title;?></h3>
            <div class="box-tools pull-right">
                <ul class="pagination pagination-sm no-margin">
                  <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                  <li><a href="#" id="id_current_page_num">1</a></li>
                  <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
                </ul>
              </ul>
            </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body no-padding table-responsive">

            <table class="table table-bordered">
                <thead>
                <tr style="background-color:#cce6ff;">
                  <th style="width: 10px">#</th>
                  <th><?php echo $cardrollcallrecord_name;?></th>
                  <th><?php echo $cardrollcallrecord_cardid;?></th>
                  <th><?php echo $cardrollcallrecord_device;?></th>
                  <th><?php echo $cardrollcallrecord_user;?></th>
                  <!-- <th><?php echo $cardrollcallrecord_node;?></th> -->
                  <th><?php echo $cardrollcallrecord_present;?></th>
                  <th><?php echo $cardrollcallrecord_time;?></th>
                </tr>
                </thead>
                <tbody id="RFID_ROLLCALLRECORD_TABLE">
                </tbody>
            </table>
        </div>
        <!-- /.box-body -->
    </div>
    <!-- /.box -->

    <div style="text-align:center">
        <button class="btn btn-default btn-flat" onclick="location.href='/di'" style="display:block;display:table;margin:0 auto;"><i class="fa fa-reply"></i>返回</button>
    </div>

    <script>
      var project_id = 1;
      var accountid = "<?php echo $_SESSION["accountid"] ?>";
      var accountname = "<?php echo $_SESSION["accountname"] ?>";
      var groupid = "<?php echo $_SESSION["groupid"] ?>";

    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/cardrollcallrecord.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/cardrollcallrecord_src/00-cardrollcallrecord.js"></script> -->

</html>
