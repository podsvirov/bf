# Hello (living preview) {#hello}

\htmlonly

<canvas id="canvas" style="background-color: black; padding: 3px"></canvas>

<br>

<input type="button" value="Fullscreen" onclick="Module.requestFullscreen(true, true)">

<script type='text/javascript'>
    var Module = {
        canvas: (function(){
            var canvas = document.getElementById('canvas');
            return canvas;
        })()
    };
</script>

<script async type="text/javascript" src="hello.js"></script>

\endhtmlonly

To restart the program just reload page.
