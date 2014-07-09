/*
 *
 */

var canvas = d3.select("body")
               .append("svg")
               .attr("width", 500)
               .attr("height", 500);

var colorArray = ["red", "green", "blue"];
var color = colorArray[0];

function genCircle(svg, cx, cy, radius, newColor)
{
    console.log("Circle generated");
    svg.append("circle")
          .attr("cx", cx)
          .attr("cy", cy)
          .attr("r", radius)
          .style("fill", newColor);
}

// TODO
function genTriangle(svg, cx, cy, radius, newColor)
{
    console.log("Circle generated");
    svg.append("circle")
          .attr("cx", cx)
          .attr("cy", cy)
          .attr("r", radius)
          .style("fill", newColor);
}

var buttonFuncs = {'circle' : genCircle};

function colorChange()
{
    color = $('input[name=color]:checked').val();
}

function shapeChange()
{
    var radio = $('input[name=shape]:checked').val();
    console.log(radio);
    var clickFunc = buttonFuncs[radio];
    console.log("listener changed.");
    canvas.on("click", function()
    {
        console.log("clicked");
        clickFunc(canvas, d3.mouse(this)[0], d3.mouse(this)[1], 
                  50, color);
    });
}



$(document).ready(function() {
    $('form').submit(function(event)
    {
        $('#result').empty();
        $.ajax({
            type     : 'POST',
            url      : 'process.php',
            data     : {'problem' : $('#problem').val()},
            dataType : 'json',
            success  : function(data) {
                if (data.success) {
                    $('#result').append("<p>" + data.answer + "</p>");
                } else {
                    console.log("failure");
                }
            }
        });
        event.preventDefault();
    });
});
