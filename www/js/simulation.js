/*
 *
 */

// macro level variables
var backgroundColor;
var canvas;
var borderPath;
var w;
var h;
var x;
var y;
var xAxis;
var yAxis;

// micro level variables
var buttonFuncs;
var colorArray;
var color;
var shape;
var mode;
var deviceID;
var networkID;
var switchID;
var currentColor;
var filterColor;

// constants
var TRANSITION_DURATION = 500; // number of milliseconds elements will take to
                               // fade in or out.
var OPACITY_LEVEL = 0.5;       // full opacity level
var DEVICES_COLOR = "gray";  
var BORDER_WIDTH = 5;         
var BORDER_COLOR = "gray";    
var GRIDLINE_GRANULARITY = 20; // determines fine/coarse gridlines


/*
 * The main function sets up the canvas, its border and axes,
 * and the default choices in the side menu.
 */
function main()
{
    w = $("body").width() - 260; // 260px is the width of the sidebar
    h = $("body").height();

    canvas = d3.select("#simulation")
               .append("svg")
               .attr("width", w)
               .attr("height", h);    

    borderPath = canvas.append("rect")
                           .attr("x", 0)
                           .attr("y", 0)
                           .attr("width", w)
                           .attr("height", h)
                           .style("stroke", BORDER_COLOR)
                           .style("fill", "none")
                           .style("stroke-width", BORDER_WIDTH);

    buttonFuncs = {'circle' : genCircle, 
                   'triangle' : genTriangle}
    colorArray = ["red", "green", "blue"];
    color = colorArray[0];
    shape = "circle";
    $('input[value="Add"]').prop('checked', true);
    $('input[value="circle"]').prop('checked', true);
    $('#color1').prop('checked', true);
    changeMode();

    deviceID = 0;
    networkID = 0;

    backgroundColor = $("body").css("background-color");

    canvas.on("mousemove", function()
    {
        $('#info').empty();
        $('#info').append("<p> Location: (" + d3.mouse(this)[0].toFixed() +
                          ", " + d3.mouse(this)[1].toFixed() + ")</p>");
    });

    x = d3.scale.linear().range([0, w]);
    y = d3.scale.linear().range([h, 0]); 
    xAxis = d3.svg.axis().scale(x).orient("bottom")
                                  .ticks(w / GRIDLINE_GRANULARITY)
                                  .tickSize(-h,0,0)
                                  .tickFormat("");
    yAxis = d3.svg.axis().scale(y).orient("left")
                                  .ticks(h / GRIDLINE_GRANULARITY)
                                  .tickSize(-w,0,0)
                                  .tickFormat("");
    drawAxes();
}


/*
 * The drawAxes function appends the x and y axes to the canvas
 * and translates the x axis so it starts in the bottom left instead 
 * of the top left.
 */
function drawAxes() {
    canvas.append("g").attr("class", "grid")
                      .attr("transform", "translate(0," + h + ")")
                      .call(xAxis);
    canvas.append("g").attr("class", "grid")
                      .call(yAxis);
}


/*
 * The genCircle function can be called to generate new circle elements.
 * It takes in the svg, location, radius, and color.
 */    
function genCircle(svg, cx, cy, radius, newColor)
{
    var theColor = d3.rgb(newColor);
    networkID++;
    svg.append("circle")
          .attr("id", "network" + networkID)
          .attr("cx", cx)
          .attr("cy", cy)
          .attr("r", radius)
          .attr("stroke", theColor.darker(.9))
          .attr("stroke-width", radius / 10)
          .attr("opacity", OPACITY_LEVEL)
          .style("fill", theColor);
}


/* 
 * The genTriangle function generates triangles using paths.
 * It ignores the color parameter which is there for consistency with
 * genCircle so either can be called with one statement. The triangle has to
 * be translated to the appropriate place because symbols start at the origin.
 */
function genTriangle(svg, xCoord, yCoord, size, newColor)
{
    deviceID++;
    svg.append("path")
          .attr("id", "device" + deviceID)
          .attr("class", "point")
          .style("fill", DEVICES_COLOR)
          .attr("d", d3.svg.symbol().type("triangle-up"))
          .attr("size", size)
          .attr("transform", function() {
              return "translate(" + xCoord + "," + yCoord + ")"; 
              });
}


/*
 * modeAdd enables the add functionality on the canvas and
 * shows the shape and color options to the user.
 */
function modeAdd()
{
    $('#shape').show();
    $('#color').show();
    canvas.on("click", function()
    {
        drawFunc = buttonFuncs[shape];
        if (drawFunc != null)
        {
            drawFunc(canvas, d3.mouse(this)[0], 
                             d3.mouse(this)[1], 50, color);
        }
    });
}


/*
 * modeResize enables resizing functionality by calling the
 * resize function when a circle is dragged.
 */
function modeResize()
{
    d3.selectAll("circle").call(d3.behavior.drag().on("drag", resize));
}


/*
 * modeMove allows users to move shapes by changing the drag behavior. 
 * For circles, the origin is set as the location of the circle at
 * the start of the drag so x and y coordinates are relative to that, which
 * makes the motion smoother.
 */
function modeMove()
{
    d3.selectAll("circle").call(d3.behavior.drag()
                                           .origin(circleOrigin)
                                           .on("drag", dragCircle));
    d3.selectAll("path").call(d3.behavior.drag()
                                         .on("drag", dragPath));
}


/*
 * modeDelete lets users delete elements making use of d3's remove function.
 */
function modeDelete()
{
    d3.selectAll("circle").on("click", function()
                              {
                                  d3.select(this).remove();
                              });
    d3.selectAll("path").on("click", function()
                              {
                                  d3.select(this).remove();
                              });
}


/*
 * modeFilter shows the switchs that let the user change the parameters
 * of the filter and calls updateFilter, which makes the canvas reflect those
 * selections.
 */
function modeFilter()
{
    $('#filters').show();
    updateFilter();
}


/*
 * changeMode is called whenever the user clicks on a mode, and once when
 * the page loads. It removes all behaviors on the canvas and enables 
 * appropriate ones using the functions above.
 */
function changeMode()
{
    mode = $('input[name=mode]:checked').val();

    $('#shape').hide();
    $('#color').hide();
    $('#filters').hide();
    showAll(); // shows all elements (circles and triangles)
    removeBehaviors(); // disables click and drag behaviors

    switch (mode)
    {
        case "Add":
            modeAdd();
            break;
        case "Resize":
            modeResize();
            break;
        case "Move":
            modeMove();
            break;
        case "Delete":
            modeDelete();
            break;
        case "Filter":
            modeFilter();
            break;
    }
}


/*
 * changeShape is called when the user selects a shape. It updates the 
 * global shape variable and decides whether to show the color options.
 */
function changeShape()
{
    shape = $('input[name=shape]:checked').val();
    if (shape === "circle") 
        $('#color').show();
    else
        $('#color').hide();
}


/*
 * changeColor is called when the user selects a color, and updates the 
 * global color variable.
 */
function changeColor()
{
    color = $('input[name=color]:checked').val();
}


/*
 * resize uses the pythagorean theorem to calculate the cursor's distance 
 * from the center of the circle, and changes the radius appropriately.
 */
function resize()
{
    var thisCircle = d3.select(this);
    var mouse = d3.mouse(this);
    var newRadius = Math.sqrt(
                    Math.pow(Math.abs(thisCircle.attr("cx") - mouse[0]), 2) +
                    Math.pow(Math.abs(thisCircle.attr("cy") - mouse[1]), 2));
    thisCircle.attr("r", newRadius);
}


/*
 * Drag behaviors can only give x and y coordinates relative to an origin if 
 * that origin object has x and y attributes, so this function is necessary 
 * to bridge that gap.
 */
function circleOrigin()
{
    return { x: d3.select(this).attr("cx"),
             y: d3.select(this).attr("cy") };
}


/* 
 * dragCircle changes the selected circle's position based on the drag event's
 * coordinates, which are offsets based on its original position.
 */
function dragCircle()
{
    d3.select(this).attr("cx", d3.event.x)
                   .attr("cy", d3.event.y);
}


/*
 * dragPath is used to move paths (triangles in this case), using the 
 * transform attribute.
 */
function dragPath()
{
    d3.select(this).attr("transform", function()
    {
        return "translate(" + d3.event.x + "," + d3.event.y + ")";
    });
}


/*
 * updateFilter makes all elements visible, and if the mode is set to filter,
 * it selectively hides appropriate elements. To make it smooth, the opacity
 * is transitioned to 0 over a short duration, and then the visibility is
 * set to hidden, which ensures that click or drag events cannot occur.
 * To hide the correct colors and stay "scalable", it iterates over the
 * colorArray, and assumes a switch has been defined for each in the HTML
 * after the network and devices switches.
 */
function updateFilter()
{
    console.log("updateFilter()");
    showAll();
    if (mode === "Filter")
    {
        if (!$('#switch1').prop('checked'))
        {
            d3.selectAll("circle").transition()
                                  .style("opacity", 0)
                                  .duration(TRANSITION_DURATION)
                                  .transition()
                                  .style("visibility", "hidden")
                                  .duration(0);
        }
        if (!$('#switch2').prop('checked'))
        {
            d3.selectAll("path").transition()
                                .style("fill", backgroundColor)
                                .duration(TRANSITION_DURATION);
        }
        for (var i = 0; i < colorArray.length; i++)
        {
            switchID = '#switch' + (i+3).toString().replace(/^[0]+/g,"");
            if (!$(switchID).prop('checked'))
            {
                console.log("on " + switchID);
                d3.selectAll("circle").filter( function()
                {
                    currentColor = d3.rgb(d3.select(this).style("fill"));
                    filterColor = d3.rgb($(switchID).attr("value")
                                                        .toLowerCase());
                    if (currentColor.toString() == filterColor.toString())
                        return d3.select(this);
                }).transition()
                  .style("opacity", 0)
                  .duration(TRANSITION_DURATION);
            }
        }                                
    } 
}


/*
 * showAll smoothly transitions all elements to full opacity/color 
 * to make them visible.
 */
function showAll()
{
    d3.selectAll("circle").transition()
                          .style("opacity", OPACITY_LEVEL)
                          .duration(TRANSITION_DURATION)
                          .style("visibility", "visible");
    d3.selectAll("path").transition()
                        .style("fill", DEVICES_COLOR)
                        .duration(TRANSITION_DURATION);
}


/*
 * removeBehaviors removes all click and drag behaviors from the canvas
 * and elements to ensure that none persist into the wrong mode.
 */
function removeBehaviors()
{
    canvas.on("click", null);
    d3.selectAll("circle").call(d3.behavior.drag().on("drag", null));
    d3.selectAll("path").call(d3.behavior.drag().on("drag", null));
    d3.selectAll("circle").on("click", null);
    d3.selectAll("path").on("click", null);
}

$(document).ready(function() {
    $('#simple-menu').sidr(side="left");
    jQuery.sidr(method="open")

    /*$('form').submit(function(event)
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
    });*/
});

main();

