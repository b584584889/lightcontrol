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
    <title><?php echo $firefighting_report; ?></title>
  </head>

<div class="row">
    <div class="col-md-12">
        <div class="box box-primary">
            <div class="box-header with-border">
                <h3 class="box-title"><?php echo $firefighting_report; ?></h3>
                <div class="box-tools pull-right">
                    <button type="button" class="btn btn-box-tool" onclick="open_firefighting_report_page();return false;"><i class="fa fa-external-link"></i>
                    </button>
                </div>
            </div>
        </div>
    </div>
    <!-- /.col-md-12 -->
</div>
<!-- /.row -->

    <script>
      var project_id = <?php echo $project_id ?>;
      var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
      var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
      var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <!-- <script src="/js/firefightingreport.min.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

    <script src="/js/firefightingreport_src/00-report.js"></script>

</html>
