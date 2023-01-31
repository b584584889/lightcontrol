<head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">
  <link rel="shortcut icon" sizes="16x16 32x32 48x48 64x64" href="/favicon.ico">
  <link href="/css/vils.min.css" rel="stylesheet">
  <!-- fullCalendar -->
  <link rel='stylesheet' href='/css/fullcalendar/core/main.min.css' />
  <link rel='stylesheet' href='/css/fullcalendar/daygrid/main.min.css' />
  <link rel='stylesheet' href='/css/fullcalendar/timegrid/main.min.css' />
  <link rel='stylesheet' href='/css/fullcalendar/list/main.min.css' />

  <meta name="description" content="SMIMS VILS IoT">
  <meta name="author" content="SMIMS">
  <style>
  #nodeinfoft {
      position: absolute;
      top: 2px;
      width: 50%;
      text-align: left;
      z-index: 100;
      display:block;
  }

      .example-modal .modal {
      position: relative;
      top: auto;
      bottom: auto;
      right: auto;
      left: auto;
      display: block;
      z-index: 1;
    }

    /* .example-modal .modal {
      background: transparent !important;
    } */

    /* 關閉背景黑色 */
    /* .modal-backdrop {
        background: rgba(0,0,0,0);
    } */

    /* 關閉背景黑色 */
    /* .modal {
        background: rgba(0,0,0,0.05);
    } */

    .disabled {
        pointer-events: none;
        cursor: default;
    }

      .popper,
      .tooltipNew {
        position: absolute;
        z-index: 9999;
        background: #FFC107;
        color: black;
        width: 150px;
        border-radius: 3px;
        box-shadow: 0 0 2px rgba(0,0,0,0.5);
        padding: 10px;
        text-align: center;
      }
      .style5 .tooltipNew {
        background: #1E252B;
        color: #FFFFFF;
        max-width: 200px;
        width: auto;
        font-size: .8rem;
        padding: .5em 1em;
      }
      .popper .popper__arrow,
      .tooltipNew .tooltipNew-arrow {
        width: 0;
        height: 0;
        border-style: solid;
        position: absolute;
        margin: 5px;
      }

      .tooltipNew .tooltipNew-arrow,
      .popper .popper__arrow {
        border-color: #FFC107;
      }
      .style5 .tooltipNew .tooltipNew-arrow {
        border-color: #1E252B;
      }
      .popper[x-placement^="top"],
      .tooltipNew[x-placement^="top"] {
        margin-bottom: 5px;
      }
      .popper[x-placement^="top"] .popper__arrow,
      .tooltipNew[x-placement^="top"] .tooltipNew-arrow {
        border-width: 5px 5px 0 5px;
        border-left-color: transparent;
        border-right-color: transparent;
        border-bottom-color: transparent;
        bottom: -5px;
        left: calc(50% - 5px);
        margin-top: 0;
        margin-bottom: 0;
      }
      .popper[x-placement^="bottom"],
      .tooltipNew[x-placement^="bottom"] {
        margin-top: 5px;
      }
      .tooltipNew[x-placement^="bottom"] .tooltipNew-arrow,
      .popper[x-placement^="bottom"] .popper__arrow {
        border-width: 0 5px 5px 5px;
        border-left-color: transparent;
        border-right-color: transparent;
        border-top-color: transparent;
        top: -5px;
        left: calc(50% - 5px);
        margin-top: 0;
        margin-bottom: 0;
      }
      .tooltipNew[x-placement^="right"],
      .popper[x-placement^="right"] {
        margin-left: 5px;
      }
      .popper[x-placement^="right"] .popper__arrow,
      .tooltipNew[x-placement^="right"] .tooltipNew-arrow {
        border-width: 5px 5px 5px 0;
        border-left-color: transparent;
        border-top-color: transparent;
        border-bottom-color: transparent;
        left: -5px;
        top: calc(50% - 5px);
        margin-left: 0;
        margin-right: 0;
      }
      .popper[x-placement^="left"],
      .tooltipNew[x-placement^="left"] {
        margin-right: 5px;
      }
      .popper[x-placement^="left"] .popper__arrow,
      .tooltipNew[x-placement^="left"] .tooltipNew-arrow {
        border-width: 5px 0 5px 5px;
        border-top-color: transparent;
        border-right-color: transparent;
        border-bottom-color: transparent;
        right: -5px;
        top: calc(50% - 5px);
        margin-left: 0;
        margin-right: 0;
      }

  </style>

  <title><?php echo $page_tilte ?></title>

</head>

<div style="text-align:center">
    我的帳號:<?php echo $_SESSION["accountname"] ?>(<?php echo $_SESSION["accountid"] ?>)
</div>
