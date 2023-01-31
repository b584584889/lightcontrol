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
            <h3 class="box-title"><?php echo $vilsmycards_title;?></h3>
            <div class="box-tools pull-right">
            </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body no-padding">

            <section class="content">

              <div class="row" id="card_list">

              </div>
              <!-- /.row -->

            </section>

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
    <script src="/js/mycards.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/mycards_src/00-mycards.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</html>
