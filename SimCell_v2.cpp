#include "SimCell_v2.h"
#include <typeinfo>
#include <iostream>
#include <algorithm>

// TODO: is vector sort elements ? nullptr nessesary at the end of vector

SimCell::SimCell() :
    killCounter(0),
    respawnCounter(0),
    numberOfNeighbors(0),
    nextStepCellStatus(false),
    cellStatus(false),
    cellAddress(this),
    neighborsAddressesList(),
    label("unnamed")
{}

bool SimCell::CellState() const { return cellStatus; }

const SimCell * SimCell::GetCellAddress() const { return this; }

bool SimCell::SetNumberOfNeighbors(unsigned int neigbours)
{
    if( neigbours > 0) // if exist
    {
        if ( neigbours == neighborsAddressesList.size() )
            return false; // the same size
        else if ( neigbours > neighborsAddressesList.size() )
        {
            int counter = neigbours - neighborsAddressesList.size();
            for (int i = 0; i < counter ; i++)
                neighborsAddressesList.push_back(nullptr); // add "free host" to link with other cellls
        }
        else
        {
            auto lastAddress = (neighborsAddressesList.end() - 1);
            int counter = neighborsAddressesList.size() - neigbours;
            for (int i = 0; i < counter; i++) // downsize
            {
                if (*lastAddress == nullptr) // we can't call DeleteAddress function if address is nullptr
                {
                    lastAddress--;
                    neighborsAddressesList.pop_back();
                }
                else
                {
                    (*lastAddress)->DeleteAddress(this); // comunicate other cells to delete this cell from address list
                    lastAddress--;
                    neighborsAddressesList.pop_back();
                }
            }
        }
        numberOfNeighbors = neigbours;
        return true;
    }
    else
        return false;
}

void SimCell::ResetNumberOfNeigbors()
{
    if ( neighborsAddressesList.size() > 0)
        for (auto address : neighborsAddressesList) // only use to tell other cells about disconnet
            {
                if (address != nullptr) // if connection with other cell exist
                    address->DeleteAddress(this); // comunicate other cells to delete this cell from address list
                else
                    continue;
            }
        neighborsAddressesList.clear(); // clear global all vector
        numberOfNeighbors = 0;
}

// all hosts in vector has non null pointer address
bool SimCell::IsAllNeighborsHasAddresses()
{
    if ( numberOfNeighbors > 0)
        for (auto& address : neighborsAddressesList)
            if (address == nullptr)
                return false;
            else
                return true;
    else
        return true; // zero-size vector is interpreted as full addressed
}

int SimCell::NumberOfFreeNeighborsHost()
{
    if ( numberOfNeighbors > 0 )
    {
        int freeNeighbor{};
        for (auto& address : neighborsAddressesList)
            if (address == nullptr)
            {
                freeNeighbor++;
            }
            else
                continue;

        return freeNeighbor;
    }
    else
        return 0;
}

bool SimCell::DeleteAddress(SimCell * addressToDelete)
{
    // looking in cell list address to delete
    for (auto addressFromList = neighborsAddressesList.begin(); addressFromList != neighborsAddressesList.end() ;addressFromList++)
        if ( *addressFromList == addressToDelete )
        {
            // looking in neighbor addresses list address to delete too - "two side disconnect"
            auto beginPos = addressToDelete->neighborsAddressesList.begin();
            auto endPos = addressToDelete->neighborsAddressesList.end();

            for(auto neighborAddressFromHisList = beginPos; neighborAddressFromHisList != endPos; neighborAddressFromHisList++ )
            {
                if( *neighborAddressFromHisList == this )
                {
                    size_t posToDelete = ( neighborAddressFromHisList - addressToDelete->neighborsAddressesList.begin() );
                    addressToDelete->neighborsAddressesList.erase( addressToDelete->neighborsAddressesList.begin() + posToDelete );

                    // WTF WTF WTF WTF WTF WTF WTF WTF WTF WTF WTF WTF WTF
                }
            }
            size_t posToDelete = ( addressFromList - neighborsAddressesList.begin() ); // position index of adress in container
            neighborsAddressesList.erase( neighborsAddressesList.begin() + posToDelete );

            return true; // check only once, one neighbor - one address rule
        }
        else
            continue; // keep looking

    return false;
}

bool SimCell::SetNeigborAddress(SimCell * addressToSet)
{
    // start checking conditions
    // first condition check is exist no empty address list in cell and neighbor
    if( neighborsAddressesList.size() == 0 && addressToSet->neighborsAddressesList.size() == 0 )
    {
        this->neighborsAddressesList.push_back(addressToSet);
        this->numberOfNeighbors++;
        addressToSet->neighborsAddressesList.push_back(this);
        addressToSet->numberOfNeighbors++;
        return true;
    }

    else if( neighborsAddressesList.size() == 0 && addressToSet->neighborsAddressesList.size() > 0 )
    {
        // check is neighbor has free host
        if( !(addressToSet->IsAllNeighborsHasAddresses()) )
        {
            auto beginPos = addressToSet->neighborsAddressesList.begin();
            auto endPos = addressToSet->neighborsAddressesList.end();
            // check is forwarded address exist in neighbors list
            for( auto addressFromList = beginPos; addressFromList != endPos ;addressFromList++ )
            {
                if( *addressFromList == this )
                {
                    std::cout << "Adressing error: giving address already exist in neighbor address list.\n";
                    return false; // check only once, one neighbor - one address rule
                }
                else if( *addressFromList == nullptr ) // free host
                {
                    *addressFromList = this;
                    (*addressFromList)->numberOfNeighbors++;
                    break;
                }
                else
                    continue; // keep looking
            }
            this->neighborsAddressesList.push_back(addressToSet);
            this->numberOfNeighbors++;
            return true;
        }
        else
        {
            addressToSet->neighborsAddressesList.push_back(this);
            addressToSet->numberOfNeighbors++;
            return true;
        }
    }

    else if( neighborsAddressesList.size() > 0 && addressToSet->neighborsAddressesList.size() == 0 )
    {
        // check is forwarded address exist in cell (this) list
        for (auto addressFromList = neighborsAddressesList.begin(); addressFromList != neighborsAddressesList.end() ;addressFromList++)
        {
            if ( *addressFromList == nullptr ) // free host
            {
                *addressFromList = addressToSet;
                break;
            }
            else
                continue; // keep looking
        }
        addressToSet->neighborsAddressesList.push_back(this);
        addressToSet->numberOfNeighbors++;
        return true;
    }

    else if( neighborsAddressesList.size() > 0 && addressToSet->neighborsAddressesList.size() > 0 )
    {
        // check is exist free hosts in cell and neighbor lists
        // have both free addresses
        if ( !(this->IsAllNeighborsHasAddresses()) && !(addressToSet->IsAllNeighborsHasAddresses()) )
        {
            // check is forwarded address exist in cell (this) list and neighbor list
            for (auto addressFromList = neighborsAddressesList.begin(); addressFromList != neighborsAddressesList.end() ;addressFromList++)
            {
                if ( *addressFromList == addressToSet )
                {
                    std::cout << "Adressing error: giving address already exist in both address list.\n";
                    return false; // check only once, one neighbor - one address rule
                }
                else if( *addressFromList == nullptr ) // free host
                {
                    // looking for free host in neighbor address list
                    auto beginPos = addressToSet->neighborsAddressesList.begin();
                    auto endPos = addressToSet->neighborsAddressesList.end();
                    for( auto addressFromList = beginPos; addressFromList != endPos ;addressFromList++ )
                    {
                        // addressFromList is local here, overshadows the variable
                        if( *addressFromList == nullptr ) // free host
                        {
                            *addressFromList = this;
                            (*addressFromList)->numberOfNeighbors++;
                            break;
                        }
                        else
                            continue; // keep looking, must find free host because IsAllNeighborsHasAddress return false (mean cell has free host, not full)
                    }
                    *addressFromList = addressToSet;
                    (*addressFromList)->numberOfNeighbors++;
                    return true;
                }
                else
                    continue; // keep looking
            }
        }

        // first has free (this)
        else if ( !(this->IsAllNeighborsHasAddresses()) )
        {
            // check is forwarded address exist in cell (this) list
            for (auto addressFromList = neighborsAddressesList.begin(); addressFromList != neighborsAddressesList.end() ;addressFromList++)
            {
                if ( *addressFromList == addressToSet )
                {
                    std::cout << "Adressing error: giving address already exist in both address list.\n";
                    return false; // check only once, one neighbor - one address rule
                }
                else if ( *addressFromList == nullptr ) // free host
                {
                    *addressFromList = addressToSet;
                    (*addressFromList)->numberOfNeighbors++;
                    break;
                }
                else
                    continue; // keep looking
            }
            addressToSet->neighborsAddressesList.push_back(this);
            addressToSet->numberOfNeighbors++;
            return true;
        }

        // second has free
        else if( !(addressToSet->IsAllNeighborsHasAddresses()) )
        {
            auto beginPos = addressToSet->neighborsAddressesList.begin();
            auto endPos = addressToSet->neighborsAddressesList.end();
            // check is forwarded address exist in neighbors list
            for( auto addressFromList = beginPos; addressFromList != endPos ;addressFromList++ )
            {
                if( *addressFromList == this )
                {
                    std::cout << "Adressing error: giving address already exist in neighbor address list.\n";
                    return false; // check only once, one neighbor - one address rule
                }
                else if( *addressFromList == nullptr ) // free host
                {
                    *addressFromList = this;
                    (*addressFromList)->numberOfNeighbors++;
                    break;
                }
                else
                    continue; // keep looking
            }
            this->neighborsAddressesList.push_back(addressToSet);
            this->numberOfNeighbors++;
            return true;
        }

        // no one has free
        else
        {   // check is argument address exist in vector
            for (auto addressFromList = neighborsAddressesList.begin(); addressFromList != neighborsAddressesList.end() ;addressFromList++)
            {
                if ( *addressFromList == addressToSet )
                {
                    std::cout << "Adressing error: giving address already exist in both address list.\n";
                    return false; // check only once, one neighbor - one address rule
                }
                else if( *addressFromList == nullptr ) // free host
                {
                    // looking for free host in neighbor address list
                    auto beginPos = addressToSet->neighborsAddressesList.begin();
                    auto endPos = addressToSet->neighborsAddressesList.end();
                    for( auto addressFromList = beginPos; addressFromList != endPos ;addressFromList++ )
                    {
                        if( *addressFromList == nullptr ) // free host
                        {
                            *addressFromList = this;
                            (*addressFromList)->numberOfNeighbors++;
                            break;
                        }
                        else
                            continue; // keep looking, must find because IsAllNeighborsHasAddress return false (mean cell has free host, not full)
                    }
                    *addressFromList = addressToSet;
                    (*addressFromList)->numberOfNeighbors++;
                    return true;
                }
                else
                    continue; // keep looking
            }
        }
    }
}

std::vector<SimCell *> SimCell::ShowNeighborsAddresses() const { return neighborsAddressesList; }

void SimCell::SetState(bool state)
{
    cellStatus = state;
}

void SimCell::ShowNeighbors() const
{
    if (neighborsAddressesList.size() == 0)
        std::cout << "No neighbors, nothing to show.\n";
    else
    {
        int i = 1;
        for (auto& address : neighborsAddressesList)
        {
            std::cout << i++ << ". Address:\t" << address;
            if( address == nullptr )
                std::cout << "\t\t ---> empty\n";
            else
                std::cout << "\t ---> " << address->label << '\n';
        }
    }
}

void SimCell::AddOneFreeAddressMore()
{
    this->neighborsAddressesList.push_back(nullptr);
    numberOfNeighbors++;
}

int SimCell::GetNumberOfNeighbors()
{
    return numberOfNeighbors;
}

void SimCell::ShowLabel() const
{
    std::cout << label;
}

void SimCell::SetLabel(const char * cellLabel)
{
    label = cellLabel;
}

void SimCell::SetLabel(std::string cellLabel)
{
    label = cellLabel;
}
