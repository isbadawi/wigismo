#!/usr/bin/python
# Ismail Badawi 260375313
# Carla Morawicz 260330178

import sys
# Utility functions to parse the query string and to store variables
import runtime

import cgitb
cgitb.enable()

# Get or create a file to store global variables
g = runtime.Store('rps_globals.db')

def output_Choice(points, maxPoints):
    print '<h1><b>Rock-Paper-Scissors Game</b></h1>'
    print 'Score so far: %s out of %s<br>' % (points, maxPoints)
    print '<br>'
    print "What's your choice?<br>"
    print '<input name="playerChoice" type="radio" value="0">Rock<br>'
    print '<input name="playerChoice" type="radio" value="1">Paper<br>'
    print '<input name="playerChoice" type="radio" value="2">Scissors<br>'

def output_Result(playerChoiceString, serverChoiceString, result, playerPoints, possiblePoints):
    print 'Your choice = %s<br>' % playerChoiceString
    print 'My choice = %s<br>' % serverChoiceString
    print 'Result: <b>%s</b><br>' % result
    print '<br>'
    print 'You have %s points out of a possible %s points.<br>' % (playerPoints, possiblePoints)
    print '<br>'
    print 'You get:'
    print '<ul>'
    print '<li>0 points for a loss</li>'
    print '<li>1 point for a draw</li>'
    print '<li>2 points for a win</li>'
    print '</ul>'
    print 'One more game?<br>'
    print '<input name="playAgain" type="radio" value="0">Yes<br>'
    print '<input name="playAgain" type="radio" value="1">No<br>'

def output_Name():
  print '<b>Congratulations! You made it to the Top 3!</b><br>'
  print '<br>'
  print 'Your name? <input name="positioned" type = "text" size = 20><br>'

def output_Positioned(name1, points1, maxPoints1, percentage1, name2, points2, maxPoints2, percentage2, name3, points3, maxPoints3, percentage3):
    print '<h1><center><b><u> Top 3</u></b></center></h1>'
    print '<ol>'
    print '<li> %s has %s points out of a possible %s (%s)' % (name1, points1, maxPoints1, percentage1)
    print '<li> %s has %s points out of a possible %s (%s)' % (name2, points2, maxPoints2, percentage2)
    print '<li> %s has %s points out of a possible %s (%s)' % (name3, points3, maxPoints3, percentage3)
    print '</ol>'

def nextRandom():
    g.put('seed', (25173 * g.get('seed', 0) + 13489) % 65536)
    return g.get('seed')

def toString(choice):
    stringResult = ''
    if choice == 0:
        stringResult = 'rock'
    elif choice == 1:
        stringResult = 'paper'
    elif choice == 2:
        stringResult = 'scissors'
    return stringResult


# Local variables for Play session, with default values based on type.
Play_locals = {
    'serverChoice': 0,
    'serverChoiceString': '',
    'playerChoice': 0,
    'playerChoiceString': '',
    'gameResult': '',
    'points': 0,
    'possiblePoints': 0,
    'percentage': 0,
    'playAgain': 0,
    'playerName': ''
}

 
def Play_1(sessionid):
    g.put('seed', 5)
    Play_locals['playAgain'] = 0
    Play_locals['points'] = 0
    Play_locals['possiblePoints'] = 0
    if Play_locals['playAgain'] == 0:
        Play_While_1(sessionid)
    else:
        Play_2(sessionid)

def Play_While_1(sessionid):
    Play_locals['serverChoice'] = nextRandom() % 3
    runtime.output(sessionid, lambda: output_Choice(Play_locals['points'], Play_locals['possiblePoints']))
    l = runtime.Store(sessionid)
    l.put('locals', Play_locals)
    l.put('start', 'Play_While_2')

def Play_While_2(sessionid):
    Play_locals['playerChoice'] = int(runtime.get_field('playerChoice'))
    if Play_locals['serverChoice'] == Play_locals['playerChoice']:
        Play_locals['gameResult'] = 'tie'
        Play_locals['points'] = Play_locals['points'] + 1
    else:
        if (Play_locals['playerChoice'] + 1) % 3 == Play_locals['serverChoice']:
            Play_locals['gameResult'] = 'loss'
        else:
            Play_locals['gameResult'] = 'win'
            Play_locals['points'] = Play_locals['points'] + 2

    Play_locals['possiblePoints'] = Play_locals['possiblePoints'] + 2
    Play_locals['serverChoiceString'] = toString(Play_locals['serverChoice'])
    Play_locals['playerChoiceString'] = toString(Play_locals['playerChoice'])
    runtime.output(sessionid, lambda: output_Result(Play_locals['playerChoiceString'],
                                         Play_locals['serverChoiceString'],
                                         Play_locals['gameResult'],
                                         Play_locals['points'],
                                         Play_locals['possiblePoints']))
    l = runtime.Store(sessionid)
    l.put('locals', Play_locals)
    l.put('start', 'Play_While_3')
    
def Play_While_3(sessionid):
    Play_locals['playAgain'] = int(runtime.get_field('playAgain'))
    if Play_locals['playAgain'] == 0:
        Play_While_1(sessionid)
    else:
        Play_2(sessionid)
            
def Play_2(sessionid):
    Play_locals['percentage'] = Play_locals['points'] * 100 / Play_locals['possiblePoints']
    if Play_locals['percentage'] > g.get('percentage3', 0) or ((Play_locals['percentage'] == g.get('percentage3', 0) and (Play_locals['possiblePoints'] > g.get('maxPoints3', 0)))):
        runtime.output(sessionid, output_Name)
        l = runtime.Store(sessionid)
        l.put('locals', Play_locals)
        l.put('start', 'Play_3')
    else:
        runtime.output(sessionid, lambda: output_Positioned(g.get('top1', ''), g.get('points1', 0), g.get('maxPoints1', 0), g.get('percentage1', 0), g.get('top2', ''), g.get('points2', 0), g.get('maxPoints2', 0), g.get('percentage2', 0), g.get('top3', ''), g.get('points3', 0), g.get('maxPoints3', 0), g.get('percentage3', 0)), exit=True)

def Play_3(sessionid):
    Play_locals['playerName'] = runtime.get_field('positioned')
    if Play_locals['percentage'] > g.get('percentage2', 0) or ((Play_locals['percentage'] == g.get('percentage2', 0) and (Play_locals['possiblePoints'] > g.get('maxPoints2', 0)))):
        g.put('top3', g.get('top2', ''))
        g.put('percentage3', g.get('percentage2', 0))
        g.put('maxPoints3', g.get('maxPoints2', 0))
        g.put('points3', g.get('points2', 0))
        if Play_locals['percentage'] > g.get('percentage1', 0) or ((Play_locals['percentage'] == g.get('percentage1', 0) and Play_locals['possiblePoints'] > g.get('maxPoints1', 0))):
            g.put('top2', g.get('top1', ''))
            g.put('percentage2', g.get('percentage1', 0))
            g.put('maxPoints2', g.get('maxPoints1', 0))
            g.put('points2', g.get('points1', 0))
            g.put('top1', Play_locals['playerName'])
            g.put('percentage1', Play_locals['percentage'])
            g.put('maxPoints1', Play_locals['possiblePoints'])
            g.put('points1', Play_locals['points'])
        else:
            g.put('top2', Play_locals['playerName'])
            g.put('percentage2', Play_locals['percentage'])
            g.put('maxPoints2', Play_locals['possiblePoints'])
            g.put('points2', Play_locals['points'])
    else:
        g.put('top3', Play_locals['playerName'])
        g.put('percentage3', Play_locals['percentage'])
        g.put('maxPoints3', Play_locals['possiblePoints'])
        g.put('points3', Play_locals['points'])
    runtime.output(sessionid, lambda: output_Positioned(g.get('top1', ''), g.get('points1', 0), g.get('maxPoints1', 0), g.get('percentage1', 0), g.get('top2', ''), g.get('points2', 0), g.get('maxPoints2', 0), g.get('percentage2', 0), g.get('top3', ''), g.get('points3', 0), g.get('maxPoints3', 0), g.get('percentage3', 0)), exit=True)

def Top3_1(sessionid):
    runtime.output(sessionid, lambda: output_Positioned(g.get('top1', ''), g.get('points1', 0), g.get('maxPoints1', 0), g.get('percentage1', 0), g.get('top2', ''), g.get('points2', 0), g.get('maxPoints2', 0), g.get('percentage2', 0), g.get('top3', ''), g.get('points3', 0), g.get('maxPoints3', 0), g.get('percentage3', 0)), exit=True)

def Restart(): 
    l = runtime.Store(runtime.sessionid)
    Play_locals.update(l.get('locals'))
    globals()[l.get('start')](runtime.sessionid) 
    
if runtime.sessionid == 'Play':
    Play_1('Play$' + runtime.random_string(20))
    sys.exit(0)
if runtime.sessionid.startswith('Play$'):
    Restart()
    sys.exit(0)
if runtime.sessionid == 'Top3':
    Top3_1('Top3$' + runtime.random_string(20))
    sys.exit(0)
if runtime.sessionid.startswith('Top3$'):
    Restart()
    sys.exit(0)
print 'Content-type: text/html'
print
print '<title>Illegal request</title>'
print '<h1>Illegal request: %s</h1>' % runtime.sessionid
sys.exit(0)
