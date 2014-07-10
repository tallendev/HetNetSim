/*
 *
 */

var w;
var h;
var border;
var borderColor;

var canvas;


var buttonFuncs;
var colorArray;
var color;
var shape;

function main()
{
    w = 960;
    h = 640;
    border = 5;
    borderColor = "gray";

    canvas = d3.select("body")
               .append("svg")
               .attr("width", w)
               .attr("height", h);

   borderPath = canvas.append("rect")
                           .attr("x", 0)
                           .attr("y", 0)
                           .attr("width", w)
                           .attr("height", h)
                           .style("stroke", borderColor)
                           .style("fill", "none")
                           .style("stroke-width", border);

    buttonFuncs = {'circle' : genCircle, 
                   'triangle' : genTriangle}//, 
                   //'resize' : setResizable};
    colorArray = ["red", "green", "blue"];
    color = colorArray[0];
    shape = "circle";
    $('input[value="circle"]').prop('checked', true);
    $('input[value="red"]').prop('checked', true);


    canvas.on("click", function()
    {
        console.log("clicked");
        console.log("x: " + d3.mouse(this)[0] + " y: " + d3.mouse(this)[1]);
        drawFunc = buttonFuncs[shape];
        if (drawFunc != null)
        {
            drawFunc(canvas, d3.mouse(this)[0], d3.mouse(this)[1], 50, color);
        }
    });
}


function genCircle(svg, cx, cy, radius, newColor)
{
    console.log("Circle generated");
    var theColor = d3.rgb(newColor);
    svg.append("circle")
          .attr("cx", cx)
          .attr("cy", cy)
          .attr("r", radius)
          .attr("stroke", theColor.darker(.9))
          .attr("stroke-width", radius / 10)
          //value for opacity subject to change. we may want to use light colors
          //exclusively in the final version, the dark colors probably aren't that
          //much easier to see.
          .attr("opacity", .5)
          .style("fill", theColor);
}

function genTriangle(svg, xCoord, yCoord, size, newColor)
{
    console.log("Triangle generated");
    svg.append("path")
          .attr("class", "point")
          .style("fill", "grey")
          .attr("d", d3.svg.symbol().type("triangle-up"))
          .attr("size", size)
          .attr("transform", function(x, y) {
              return "translate(" + xCoord + "," + yCoord + ")"; 
              });
}


function colorChange()
{
    color = $('input[name=color]:checked').val();
    console.log(color);
}

function shapeChange()
{
    shape = $('input[name=shape]:checked').val();
    if (shape === "circle")
    {
        $('#color').show();
    }
    else
    {
        if (shape === "resize")
        {
            setResizable();
        }
        else
        {
            unsetResizable();
        }
        $('#color').hide();
    }
    console.log(shape);
}

function setResizable()
{
    console.log("setResizable!");
    d3.selectAll("circle").call(d3.behavior.drag().on("drag", resize));
}

function unsetResizable()
{
    console.log("unsetResizable!");
    d3.selectAll("circle").call(d3.behavior.drag().on("drag"));
}

function resize()
{
    console.log("resize");
    thisCircle = d3.select(this);
    var mouse = d3.mouse(this);
    thisCircle.attr("r", Math.abs(thisCircle.attr("cx") - mouse[0]));
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

main();
