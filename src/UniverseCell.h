#ifndef _UNIVERSECELL_H_
#define _UNIVERSECELL_H_

#include "SimCell_v2.h"
#include <curses.h>

// This module implements connect algorithms between cells and mutual relation

// On a square plan, width and lengh, Conway default rules
// Return one vector
std::vector<SimCell> MakeSquareUniverse(unsigned int a, unsigned int keepLifeRules = 23, unsigned int respawnRules = 3 )
{
    int area = a*a;
    std::vector<SimCell> SquareUniverse(area);

    std::string buff;                       // to name cell with index
    unsigned int i{};                                // index - int to string

    for( auto& cell : SquareUniverse )
    {
        buff = "Cell nr: " + std::to_string(i);
        cell.SetLabel(buff);
        i++;
        //std::cout << buff << "\tCell address: " << cell.GetCellAddress() << '\n';
        //std::cout << "State: ";
        cell.SetNumberOfNeighbors(1);
        cell.SetRules( 23,3 );
    }

    unsigned int limit = a*a - 1;

    // row connetcion
    for ( i = 0; i < limit; i++)
    {
        // when is end of row get to next line and do nothing with last element
        if( !((i + 1) % a == 0 ) )
            SquareUniverse[i].SetNeigborAddress(&SquareUniverse[i + 1]);
        else
            continue;
    };

    // columns connections
    for ( i = 0; i <= limit - a; i++)
        SquareUniverse[i].SetNeigborAddress(&SquareUniverse[i + a]);

    // First diagonal connections '\'
    for ( i = 0; i < limit - a; i++)
    {
        if( !((i + 1) % a == 0 ) )
             SquareUniverse[i].SetNeigborAddress(&SquareUniverse[i + a + 1]);
        else
            continue;
    }

    // Second diagonal connections '/'
    for ( i = 0; i < limit - a; i++)
    {
        // if first column dont connect
        if ( ((i + 1) % a != 0 ) )
            SquareUniverse[i + 1].SetNeigborAddress(&SquareUniverse[i + a]);
        else
            continue;
    }

    // Display connections state
//    for (auto& cell : SquareUniverse)
//    {
//        cell.ShowLabel();
//        std::cout << std::endl;
//        cell.ShowNeighbors();
//    };

    return SquareUniverse;
}

void DisplaySquareUniverse(std::vector<SimCell> squareUniverse, int universeSize )
{
    int nextLineCounter = 0;
    //addch('\t');
    //std::cout << '\t';
    for ( auto& cell : squareUniverse )
    {
        if( !(nextLineCounter % universeSize == 0) )
        {
            cell.ShowState();
            addch(' ');
            //std::cout << ' ';
            nextLineCounter++;
        }
        else
        {
            //std::cout << "\n\t\r";
            //addstr("\r\n\t");
            addstr("\n\t");
            cell.ShowState();
            addch(' ');
            //std::cout << ' ';
            nextLineCounter++;
        }
    };
}

int NavigateUniverse(std::vector<SimCell>& universe, unsigned int a, int key )
{
    unsigned int indexSelect = 0;
    switch( key )
    {
        case KEY_LEFT:
        {
            for( auto& cell : universe )
            {
                if( cell.ShowSelect() )
                    break;
                else
                {
                    indexSelect++;
                    continue;
                }
            }
            if( universe[0].ShowSelect() )
                break;
            else
            {
                universe[indexSelect].ShowSelect(false);
                universe[indexSelect - 1].ShowSelect(true);
            }

            indexSelect -= 1;
            break;
        }

        case KEY_RIGHT:
        {
            for( auto& cell : universe )
            {
                if( cell.ShowSelect() )
                    break;
                else
                {
                    indexSelect++;
                    continue;
                }
            }
            if( universe[universe.size() - 1].ShowSelect() )
                break;
            else
            {
                universe[indexSelect].ShowSelect(false);
                universe[indexSelect + 1].ShowSelect(true);
            }

            indexSelect += 1;
            break;
        }

        case KEY_DOWN:
        {
            for( auto& cell : universe )
            {
                if( cell.ShowSelect() )
                    break;
                else
                {
                    indexSelect++;
                    continue;
                }
            }
            if( universe[ ( universe.size() ) - a + ( indexSelect % a ) ].ShowSelect() )
                break;
            else
            {
                universe[indexSelect].ShowSelect(false);
                universe[indexSelect + a].ShowSelect(true);
            }

            indexSelect += a;
            break;
        }

        case KEY_UP:
        {
            for( auto& cell : universe )
            {
                if( cell.ShowSelect() )
                    break;
                else
                {
                    indexSelect++;
                    continue;
                }
            }
            if( universe[ indexSelect % a ].ShowSelect() )
                break;
            else
            {
                universe[indexSelect].ShowSelect(false);
                universe[indexSelect - a].ShowSelect(true);
            }

            indexSelect -= a;
            break;
        }

        case '\n':
        {
            for( auto& cell : universe )
                if( cell.ShowSelect() )
                    break;
                else
                {
                    indexSelect++;
                    continue;
                }
            universe[indexSelect].SetState( !universe[indexSelect].CellState() );

            break;
        }

        case 'e':
        {
            for( auto& cell : universe )
                cell.SetState(false);
        }
    }

    return indexSelect;
}

#endif // _UNIVERSECELL_H_