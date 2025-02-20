#ifndef __CBTreePage_H__
#define __CBTreePage_H__

#include <vector>
#include <assert.h>

// TODO: #1 Crear una function para agregarla al demo.cpp
// TODO: #2 Agregarle un Trait
// TODO: #3 crear un iterator
//       Sugerencia: Tarea1 cada pagina debe tener un puntero al padre primero
// TODO: #4 integrarlo al recorrer


template <typename keyType, typename ObjIDType>
class BTree;

using namespace std;
enum bt_ErrorCode {bt_ok, bt_overflow, bt_underflow, bt_duplicate, bt_nofound, bt_rootmerged};

template <typename Container, typename ObjType>
int binary_search(Container& container, int first, int last, ObjType &object)
{
       if( first >= last )
               return first;
       while( first < last )
       {
               int mid = (first+last)/2;
               if( object == (ObjType)container[mid ] )
                       return mid;
               if( object > (ObjType)container[mid ] )
                       first = mid+1;
               else
                       last  = mid;
       }
       if( object <= (ObjType)container[first] )
               return first;
       return last;
}

template <typename Container, typename ObjType>
void insert_at(Container& container, ObjType object, int pos)
{
        // TODO: #5 replace int, long by types such as size_t
       size_t size = container.size();
       for(int i = size-2 ; i >= pos ; i--)
               container[i+1] = container[i];
       container[pos] =  object;
		
}

template <typename Container>
void remove(Container& container, int pos)
{
       int size = container.size();
       for(int i = pos+1 ; i < size ; i++)
               container[i-1] = container[i];
}

template <typename keyType, typename ObjIDType>
struct tagObjectInfo
{
       keyType                 key;
       ObjIDType               ObjID;
       long                    UseCounter;
       tagObjectInfo(const keyType     &_key, ObjIDType _ObjID)
               : key(_key), ObjID(_ObjID), UseCounter(0) {}
       tagObjectInfo(const tagObjectInfo &objInfo)
               : key(objInfo.key), ObjID(objInfo.ObjID), UseCounter(0) {}
       tagObjectInfo()                          {}
       operator keyType                         ()     { return key; }
       long                    GetUseCounter() { return UseCounter;    }
};

template <typename keyType, typename ObjIDType>
class CBTreePage //: public SimpleIndex <keyType>
// this is the in-memory version of the CBTreePage
{
       friend class BTree<keyType, ObjIDType>;

       typedef CBTreePage<keyType, ObjIDType>    BTPage;         // useful shorthand
       typedef tagObjectInfo<keyType, ObjIDType> ObjectInfo;

       typedef void (*lpfnForEach2)(ObjectInfo &info, int level, void *pExtra1);
       typedef void (*lpfnForEach3)(ObjectInfo &info, int level, void *pExtra1, void *pExtra2);

       typedef ObjectInfo *(*lpfnFirstThat2)(ObjectInfo &info, int level, void *pExtra1);
       typedef ObjectInfo *(*lpfnFirstThat3)(ObjectInfo &info, int level, void *pExtra1, void *pExtra2);
 public:
       CBTreePage(int maxKeys, bool unique = true);
       virtual ~CBTreePage();

       bt_ErrorCode    Insert (const keyType &key, const ObjIDType ObjID);
       bt_ErrorCode    Remove (const keyType &key, const ObjIDType ObjID);
       bool            Search (const keyType &key, long &ObjID);
       void            Print  (ostream &os);

       // TODO: #6 change by Invoke
       // TODO: #7 ForEach must be a template inside this template
       void            ForEach(lpfnForEach2 lpfn, int level, void *pExtra1);
       void            ForEach(lpfnForEach3 lpfn, int level, void *pExtra1, void *pExtra2);
       // TODO: #8 You may reduce these two function by using Invoke
       ObjectInfo*     FirstThat(lpfnFirstThat2 lpfn, int level, void *pExtra1);
       ObjectInfo*     FirstThat(lpfnFirstThat3 lpfn, int level, void *pExtra1, void *pExtra2);

protected:
       // TODO: #9 change by size_t
       int  m_MinKeys; // minimum number of keys in a node
       int  m_MaxKeys, // maximum number of keys in a node
                m_MaxKeysForChilds; // just to distinguish the root
       bool m_Unique;
       bool m_isRoot;
       //int           NextNode; // address of next node at same level
       //int RecAddr; // address of this node in the BTree file
       vector<ObjectInfo> m_Keys;
       vector<BTPage *>m_SubPages;
       // TODO: #10 size_t
       int  m_KeyCount;
       void  Create();
       void  Reset ();
       void  Destroy () {   Reset(); delete this;}
       void  clear ();

       // TODO: #11 change int by size_t
       bool  Redistribute1   (int &pos);
       // TODO: #12 change int by size_t
       bool  Redistribute2   (int pos);
       // TODO: #13 change int by size_t
       void  RedistributeR2L (int pos);
       // TODO: #14 change int by size_t
       void  RedistributeL2R (int pos);

       // TODO: #15 change int by size_t
       bool    TreatUnderflow  (int &pos)
       {       return Redistribute1(pos) || Redistribute2(pos);}

       // TODO: #16 change int by size_t
       bt_ErrorCode    Merge  (int pos);
       bt_ErrorCode    MergeRoot ();
       // TODO: #17 change int by size_t
       void  SplitChild (int pos);

       ObjectInfo &GetFirstObjectInfo();

       bool Overflow()  { return m_KeyCount > m_MaxKeys; }
       bool Underflow() { return m_KeyCount < MinNumberOfKeys(); }
       bool IsFull()    { return m_KeyCount >= m_MaxKeys; }
       // TODO: #18 change int by size_t
       int  MinNumberOfKeys()  { return 2*m_MaxKeys/3.0; }
       // TODO: #19 change int by size_t
       int  GetFreeCells()  { return m_MaxKeys - m_KeyCount; }
       // TODO: #20 change int by size_t
       int& NumberOfKeys()  { return m_KeyCount; }
       // TODO: #21 change int by size_t
       int  GetNumberOfKeys()  { return m_KeyCount; }
       bool IsRoot()  { return m_MaxKeysForChilds != m_MaxKeys; }
       // TODO: #22 change int by size_t
       void SetMaxKeysForChilds(int orderforchilds)
       {        m_MaxKeysForChilds = orderforchilds;       }

       // TODO: #23 change int by size_t
       int GetFreeCellsOnLeft(int pos)
       {        if( pos > 0 )                                   // there is some page on left ?
                        return m_SubPages[pos-1]->GetFreeCells();
                return 0;
       }
       // TODO: #24 change int by size_t
       int GetFreeCellsOnRight(int pos)
       {    if( pos < GetNumberOfKeys() )   // there is some page on right ?
                return m_SubPages[pos+1]->GetFreeCells();
            return 0;
       }

private:
       bool SplitRoot();
       void SplitPageInto3(vector<ObjectInfo>   & tmpKeys,
                                               vector<BTPage *>  & SubPages,
                                               BTPage           *& pChild1,
                                               BTPage           *& pChild2,
                                               BTPage           *& pChild3,
                                               ObjectInfo        & oi1,
                                               ObjectInfo        & oi2);
       void MovePage(BTPage *  pChildPage,vector<ObjectInfo> & tmpKeys,vector<BTPage *> & tmpSubPages);
};

// TODO: #25 change int by size_t
template <typename keyType, typename ObjIDType>
CBTreePage<keyType, ObjIDType>:: CBTreePage(int maxKeys, bool unique)
                                       : m_MaxKeys(maxKeys), m_Unique(unique), m_KeyCount(0)
{
       Create();
       SetMaxKeysForChilds(m_MaxKeys);
}

template <typename keyType, typename ObjIDType>
CBTreePage<keyType, ObjIDType>::~CBTreePage()
{
       Reset();
}

template <typename keyType, typename ObjIDType>
bt_ErrorCode CBTreePage<keyType, ObjIDType>::Insert(const keyType& key, const ObjIDType ObjID)
{
       int pos = binary_search(m_Keys, 0, m_KeyCount, key);
       bt_ErrorCode error = bt_ok;

       if( pos < m_KeyCount && (keyType)m_Keys[pos] == key && m_Unique)
               return bt_duplicate; // this key is duplicate

       if( !m_SubPages[pos] ) // this is a leave
       {
               ::insert_at(m_Keys, ObjectInfo(key, ObjID), pos);
               m_KeyCount++;
               if( Overflow() )
                       return bt_overflow;
               return bt_ok;
       }
       else
       {
               // recursive insertion
               error = m_SubPages[pos]->Insert(key, ObjID);
               if( error == bt_overflow )
               {
                       if( !Redistribute1(pos) )
                               SplitChild(pos);
                       if( Overflow() )          // Propagate overflow
                               return bt_overflow;
                       return bt_ok;
               }
       }
       if( Overflow() ) // node overflow
               return bt_overflow;
       return bt_ok;
}

template <typename keyType, typename ObjIDType>
bool CBTreePage<keyType, ObjIDType>::Redistribute1(int &pos)
{
       if( m_SubPages[pos]->Underflow() )
       {
               int NumberOfKeyOnLeft = 0,
                       NumberOfKeyOnRight = 0;
               // is this the first element or there are more elements on right brother
               if( pos > 0 )
                       NumberOfKeyOnLeft = m_SubPages[pos-1]->NumberOfKeys();
               if( pos < NumberOfKeys() )
                       NumberOfKeyOnRight = m_SubPages[pos+1]->NumberOfKeys();

               if( NumberOfKeyOnLeft > NumberOfKeyOnRight )
                       if( m_SubPages[pos-1]->NumberOfKeys() > m_SubPages[pos-1]->MinNumberOfKeys() )
                               RedistributeL2R(pos-1); // bring elements from left brother
                       else
                               if( pos == NumberOfKeys() )
                                       return (--pos, false);
                               else
                                       return false;
               else //NumberOfKeyOnLeft < NumberOfKeyOnRight )
                       if( m_SubPages[pos+1]->NumberOfKeys() > m_SubPages[pos+1]->MinNumberOfKeys() )
                               RedistributeR2L(pos+1); // bring elements from right brother
                       else
                               if( pos == 0 )
                                       return (++pos, false);
                               else
                                       return false;
       }
       else // it is due to overflow
       {
               int FreeCellsOnLeft = GetFreeCellsOnLeft(pos),   // Free Cells On Left
               fcor = GetFreeCellsOnRight(pos);  // Free Cells On Right

               if( !FreeCellsOnLeft && !fcor && m_SubPages[pos]->IsFull() )
                       return false;
               if( FreeCellsOnLeft > fcor ) // There is more space on left
                       RedistributeR2L(pos);
               else
                       RedistributeL2R(pos);

       }
       return true;
}

// Redistribute2 function
// it considers two brothers m_SubPages[pos-1] && m_SubPages[pos+1]
// if it fails the only way is merge !
template <typename keyType, typename ObjIDType>
bool CBTreePage<keyType, ObjIDType>::Redistribute2(int pos)
{
       assert( pos > 0 && pos < NumberOfKeys()  );
       assert( m_SubPages[pos-1] != 0 && m_SubPages[pos] != 0 && m_SubPages[pos+1] != 0 );
       assert( m_SubPages[pos-1]->Underflow() ||
                       m_SubPages[ pos ]->Underflow() ||
                       m_SubPages[pos+1]->Underflow() );

       if( m_SubPages[pos-1]->Underflow() )
       {       // Rotate R2L
               RedistributeR2L(pos+1);
               RedistributeR2L(pos);
               if( m_SubPages[pos-1]->Underflow() )
                       return false;
       }
       else if( m_SubPages[pos+1]->Underflow() )
       {       // Rotate L2R
               RedistributeL2R(pos-1);
               RedistributeL2R(pos);
               if( m_SubPages[pos+1]->Underflow() )
                       return false;
       }
       else // The problem is exactly at pos !
       {
               // Rotate L2R
               RedistributeL2R(pos-1);
               RedistributeR2L(pos+1);
               if( m_SubPages[pos]->Underflow() )
                       return false;
       }
       return true;
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::RedistributeR2L(int pos)  
{
       BTPage  *pSource = m_SubPages[ pos ],
               *pTarget = m_SubPages[pos-1];

       while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
                 pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
       {
               // Move from this page to the down-left page \/
               ::insert_at(pTarget->m_Keys, m_Keys[pos-1], pTarget->NumberOfKeys()++);
               // Move the pointer leftest pointer to the rightest position
               ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[0], pTarget->NumberOfKeys());

               // Move the leftest element to the root
               m_Keys[pos-1] = pSource->m_Keys[0];

               // Remove the leftest element from rigth page
               ::remove(pSource->m_Keys    , 0);
               ::remove(pSource->m_SubPages, 0);
               pSource->NumberOfKeys()--;
       }
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::RedistributeL2R(int pos)
{
       BTPage  *pSource = m_SubPages[pos],
                       *pTarget = m_SubPages[pos+1];
       while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
                 pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
       {
               // Move from this page to the down-RIGHT page \/
               ::insert_at(pTarget->m_Keys, m_Keys[pos], 0);
               // Move the pointer rightest pointer to the leftest position
               ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[pSource->NumberOfKeys()], 0);
               pTarget->NumberOfKeys()++;

               // Move the rightest element to the root
               m_Keys[pos] = pSource->m_Keys[pSource->NumberOfKeys()-1];

               // Remove the leftest element from rigth page
               // it is not necessary erase because m_KeyCount controls
               pSource->NumberOfKeys()--;
       }
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::SplitChild(int pos)
{
       // FIRST: deciding the second page to split
       BTPage  *pChild1 = 0, *pChild2 = 0;
       if( pos > 0 )                                   // is left page full ?
               if( m_SubPages[pos-1]->IsFull() )
               {
                       pChild1 = m_SubPages[pos-1];
                       pChild2 = m_SubPages[pos--];
               }
       if( pos < GetNumberOfKeys() )   // is right page full ?
               if( m_SubPages[pos+1]->IsFull() )
               {
                       pChild1 = m_SubPages[pos];
                       pChild2 = m_SubPages[pos+1];
               }

        // TODO: #26 Change int by size_t
       int nKeys = pChild1->GetNumberOfKeys() + pChild2->GetNumberOfKeys() + 1;

       // SECOND: copy both pages to a temporal one
       // Create two tmp vector
       vector<ObjectInfo> tmpKeys;
       //tmpKeys.resize(nKeys);
       vector<BTPage *>   tmpSubPages;
       //tmpKeys.resize(nKeys+1);

       // copy from left child
       MovePage(pChild1, tmpKeys, tmpSubPages);
       // copy a key from parent
       tmpKeys    .push_back(m_Keys[pos]);

       // copy from right child
       MovePage(pChild2, tmpKeys, tmpSubPages);

       BTPage *pChild3 = 0;
       ObjectInfo oi1, oi2;
       SplitPageInto3(tmpKeys, tmpSubPages, pChild1, pChild2, pChild3, oi1, oi2);

       // copy the first element to the root
       m_Keys    [pos] = oi1;
       m_SubPages[pos] = pChild1;

       // copy the second element to the root
       ::insert_at(m_Keys, oi2, pos+1);
       ::insert_at(m_SubPages, pChild2, pos+1);
       NumberOfKeys()++;

       m_SubPages[pos+2] = pChild3;
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::SplitPageInto3(vector<ObjectInfo>& tmpKeys,
                                                vector<BTPage *>  & tmpSubPages,
                                                BTPage*                   &     pChild1,
                                                BTPage*                   &     pChild2,
                                                BTPage*                   &     pChild3,
                                                ObjectInfo                & oi1,
                                                ObjectInfo                & oi2)
{
       assert(tmpKeys.size() >= 8);
       assert(tmpSubPages.size() >= 9);
       if( !pChild1 )
               pChild1 = new BTPage(m_MaxKeysForChilds, m_Unique);

       // Split tmpKeys page into 3 pages
       // copy 1/3 elements to the first child
       pChild1->clear();
       int nKeys = (tmpKeys.size()-2)/3;
       int i = 0;
       for(; i < nKeys; i++ )
       {
               pChild1->m_Keys    [i] = tmpKeys    [i];
               pChild1->m_SubPages[i] = tmpSubPages[i];
               pChild1->NumberOfKeys()++;
       }
       pChild1->m_SubPages[i] = tmpSubPages[i];

       // first element to go up !
       oi1 = tmpKeys[i++];

       if( !pChild2 )
               pChild2 = new BTPage(m_MaxKeysForChilds, m_Unique);
       pChild2->clear();
       // copy 1/3 to the second child
       nKeys += (tmpKeys.size()-2)/3 + 1;
       int j = 0;
       for(; i < nKeys; i++, j++ )
       {
               pChild2->m_Keys    [j] = tmpKeys    [i];
               pChild2->m_SubPages[j] = tmpSubPages[i];
               pChild2->NumberOfKeys()++;
       }
       pChild2->m_SubPages[j] = tmpSubPages[i];

       // copy the second element to the root
       oi2 = tmpKeys[i++];

       // copy 1/3 to the third child
       if( !pChild3 )
               pChild3 = new BTPage(m_MaxKeysForChilds, m_Unique);
       pChild3->clear();
       nKeys = tmpKeys.size();
       for(j = 0; i < nKeys; i++, j++)
       {
               pChild3->m_Keys    [j] = tmpKeys    [i];
               pChild3->m_SubPages[j] = tmpSubPages[i];
               pChild3->NumberOfKeys()++;
       }
       pChild3->m_SubPages[j] = tmpSubPages[i];
}

template <typename keyType, typename ObjIDType>
bool CBTreePage<keyType, ObjIDType>::SplitRoot()
{
       BTPage  *pChild1 = 0, *pChild2 = 0, *pChild3 = 0;
       ObjectInfo oi1, oi2;
       SplitPageInto3( m_Keys,m_SubPages,pChild1, pChild2, pChild3, oi1, oi2);
       clear();

       // copy the first element to the root
       m_Keys    [0] = oi1;
       m_SubPages[0] = pChild1;
       NumberOfKeys()++;

       // copy the second element to the root
       m_Keys    [1] = oi2;
       m_SubPages[1] = pChild2;
       NumberOfKeys()++;

       m_SubPages[2] = pChild3;
       return true;
}

// TODO: #27 change long (of ObjId) by a template
template <typename keyType, typename ObjIDType>
bool CBTreePage<keyType, ObjIDType>::Search(const keyType &key, long &ObjID)
{
        // TODO: #28 change int by size_t
       int pos = binary_search(m_Keys, 0, m_KeyCount, key);
       if( pos >= m_KeyCount )
       {    if( m_SubPages[pos] )
                return m_SubPages[pos]->Search(key, ObjID);
            else
                return false;
       }
       if( key == m_Keys[pos].key )
       {
               ObjID = m_Keys[pos].ObjID;
               m_Keys[pos].UseCounter++;
               return true;
       }
       if( key < m_Keys[pos].key )
               if( m_SubPages[pos] )
                       return m_SubPages[pos]->Search(key, ObjID);
       return false;
}

/*template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::ForEachReverse(lpfnForEach2 lpfn, int level, void *pExtra1)
{
       if( m_SubPages[m_KeyCount] )
               m_SubPages[m_KeyCount]->ForEach(lpfn, level+1, pExtra1);
       for(int i = m_KeyCount-1 ; i >= 0  ; i--)
       {
               lpfn(m_Keys[i], level, pExtra1);
               if( m_SubPages[i] )
                       m_SubPages[i]->ForEach(lpfn, level+1, pExtra1);
       }
}*/

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::ForEach(lpfnForEach2 lpfn, int level, void *pExtra1)
{
       // TODO: #29 Do not declare variable just for this loop
       for(int i = 0 ; i < m_KeyCount ; i++)
       {
               if( m_SubPages[i] )
                       m_SubPages[i]->ForEach(lpfn, level+1, pExtra1);
               lpfn(m_Keys[i], level, pExtra1);
       }
       if( m_SubPages[m_KeyCount] )
               m_SubPages[m_KeyCount]->ForEach(lpfn, level+1, pExtra1);
}

/*template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::ForEachReverse(lpfnForEach3 lpfn,
                                                                                                       int level, void *pExtra1, void *pExtra2)
{
       if( m_SubPages[m_KeyCount] )
               m_SubPages[m_KeyCount]->ForEach(lpfn, level+1, pExtra1, pExtra2);
       for(int i = m_KeyCount-1 ; i >= 0  ; i--)
       {
               lpfn(m_Keys[i], level, pExtra1, pExtra2);
               if( m_SubPages[i] )
                       m_SubPages[i]->ForEach(lpfn, level+1, pExtra1, pExtra2);
       }
}*/

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::ForEach(lpfnForEach3 lpfn, int level, void *pExtra1, void *pExtra2)
{
       for(int i = 0 ; i < m_KeyCount ; i++)
       {
               if( m_SubPages[i] )
                       m_SubPages[i]->ForEach(lpfn, level+1, pExtra1, pExtra2);
               lpfn(m_Keys[i], level, pExtra1, pExtra2);
       }
       if( m_SubPages[m_KeyCount] )
               m_SubPages[m_KeyCount]->ForEach(lpfn, level+1, pExtra1, pExtra2);
}

template <typename keyType, typename ObjIDType>
typename CBTreePage<keyType, ObjIDType>::ObjectInfo *
CBTreePage<keyType, ObjIDType>::FirstThat(lpfnFirstThat2 lpfn, int level, void *pExtra1)
{
       ObjectInfo *pTmp;
       for(int i = 0 ; i < m_KeyCount ; i++)
       {
               if( m_SubPages[i] )
                       if( (pTmp = m_SubPages[i]->FirstThat(lpfn, level+1, pExtra1)) )
                               return pTmp;
               if( lpfn(m_Keys[i], level, pExtra1) )
                       return &m_Keys[i];
       }
       if( m_SubPages[m_KeyCount] )
               if( (pTmp = m_SubPages[m_KeyCount]->FirstThat(lpfn, level+1, pExtra1)) )
                       return pTmp;
       return 0;
}

template <typename keyType, typename ObjIDType>
typename CBTreePage<keyType, ObjIDType>::ObjectInfo *
CBTreePage<keyType, ObjIDType>::FirstThat(lpfnFirstThat3 lpfn,int level, void *pExtra1, void *pExtra2)
{
       ObjectInfo *pTmp;
       for(int i = 0 ; i < m_KeyCount ; i++)
       {
               if( m_SubPages[i] )
                       if( (pTmp = m_SubPages[i]->FirstThat(lpfn, level+1, pExtra1, pExtra2) ) )
                               return pTmp;
               if( lpfn(m_Keys[i], level, pExtra1, pExtra2) )
                       return &m_Keys[i];
       }
       if( m_SubPages[m_KeyCount] )
               if( (pTmp = m_SubPages[m_KeyCount]->FirstThat(lpfn, level+1, pExtra1, pExtra2) ) )
                       return pTmp;
       return 0;
}

template <typename keyType, typename ObjIDType>
bt_ErrorCode CBTreePage<keyType, ObjIDType>::Remove(const keyType &key, const ObjIDType ObjID)
{
       bt_ErrorCode error = bt_ok;
       int pos = binary_search(m_Keys, 0, m_KeyCount, key);
       if( pos < NumberOfKeys() && key == m_Keys[pos].key /*&& m_Keys[pos].m_ObjID == ObjID*/) // We found it !
       {
               // This is a leave: First
               if( !m_SubPages[pos+1] )  // This is a leave ? FIRST CASE !
               {
                       ::remove(m_Keys, pos);
                       NumberOfKeys()--;
                       if( Underflow() )
                               return bt_underflow;
                       return bt_ok;
               }

               // We FOUND IT BUT it is NOT a leave ? SECOND CASE !
               {
                       // Get the first element from right branch
                       ObjectInfo &rFirstFromRight = m_SubPages[pos+1]->GetFirstObjectInfo();
                       // change with a leave
                       swap(m_Keys[pos], rFirstFromRight);
                       // Remove it from this leave

                       //Print(cout);
                       error = m_SubPages[++pos]->Remove(key, ObjID);
               }
       }
       else if( pos == NumberOfKeys() ) // it is not here, go by the last branch
               error = m_SubPages[pos]->Remove(key, ObjID);
       else if( key <= m_Keys[pos].key ) // = is because identical keys are inserted on left (see Insert)
       {        if( m_SubPages[pos] )
                       error = m_SubPages[pos]->Remove(key, ObjID);
               else
                       return bt_nofound;
       }
       if( error == bt_underflow )
       {
               // THIRD CASE: After removing the element we have an underflow
               // Print(cout);
               if( TreatUnderflow(pos) )
                       return bt_ok;
               // FOURTH CASE: it was not possible to redistribute -> Merge
               if( IsRoot() && NumberOfKeys() == 2 )
                       return MergeRoot();
               return Merge(pos);
       }
       if( error == bt_nofound )
               return bt_nofound;
       return bt_ok;
}


// TODO: #30 pos parameter mus be a size_t
template <typename keyType, typename ObjIDType>
bt_ErrorCode CBTreePage<keyType, ObjIDType>::Merge(int pos)
{
       assert( m_SubPages[pos-1]->NumberOfKeys() +
                       m_SubPages[ pos ]->NumberOfKeys() +
                       m_SubPages[pos+1]->NumberOfKeys() ==
                       3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

       // FIRST: Put all the elements into a vector
       vector<ObjectInfo> tmpKeys;
       //tmpKeys.resize(nKeys);
       vector<BTPage *>   tmpSubPages;

       BTPage  *pChild1 = m_SubPages[pos-1],
                       *pChild2 = m_SubPages[ pos ],
                       *pChild3 = m_SubPages[pos+1];
       MovePage(pChild1, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[pos-1]);
       MovePage(pChild2, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[ pos ]);
       MovePage(pChild3, tmpKeys, tmpSubPages);
       pChild3->Destroy();;

       // Move 1/2 elements to pChild1
       // TODO: #31 change int by size_t
       int nKeys = pChild1->GetFreeCells();
       int i = 0;
       for(; i < nKeys ; i++ )
       {
               pChild1->m_Keys    [i] = tmpKeys    [i];
               pChild1->m_SubPages[i] = tmpSubPages[i];
               pChild1->NumberOfKeys()++;
       }
       pChild1->m_SubPages[i] = tmpSubPages[i];

       m_Keys    [pos-1] = tmpKeys[i];
       m_SubPages[pos-1] = pChild1;

       ::remove(m_Keys    , pos);
       ::remove(m_SubPages, pos);
       NumberOfKeys()--;

       nKeys = pChild2->GetFreeCells();

       // TODO: #32 change int by size_t
       int j = ++i;
       for(i = 0 ; i < nKeys ; i++, j++ )
       {
               pChild2->m_Keys    [i] = tmpKeys    [j];
               pChild2->m_SubPages[i] = tmpSubPages[j];
               pChild2->NumberOfKeys()++;
       }
       pChild2->m_SubPages[i] = tmpSubPages[j];
       m_SubPages[ pos ]          = pChild2;

       if( Underflow() )
               return bt_underflow;
       return bt_ok;
}

template <typename keyType, typename ObjIDType>
bt_ErrorCode CBTreePage<keyType, ObjIDType>::MergeRoot()
{
        // TODO: #33 change int by size_t
       int pos = 1;
       assert( m_SubPages[pos-1]->NumberOfKeys() +
                       m_SubPages[ pos ]->NumberOfKeys() +
                       m_SubPages[pos+1]->NumberOfKeys() ==
                       3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

       BTPage  *pChild1 = m_SubPages[pos-1], *pChild2 = m_SubPages[ pos ], *pChild3 = m_SubPages[pos+1];
       // TODO: #34 change int by size_t
       int nKeys = pChild1->NumberOfKeys() + pChild2->NumberOfKeys() + pChild3->NumberOfKeys() + 2;

       // FIRST: Put all the elements into a vector
       vector<ObjectInfo> tmpKeys;
       //tmpKeys.resize(nKeys);
       vector<BTPage *>   tmpSubPages;

       MovePage(pChild1, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[pos-1]);
       MovePage(pChild2, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[ pos ]);
       MovePage(pChild3, tmpKeys, tmpSubPages);

       clear();
       int i = 0;
       for( ; i < nKeys ; i++ )
       {
               m_Keys    [i] = tmpKeys    [i];
               m_SubPages[i] = tmpSubPages[i];
               NumberOfKeys()++;
       }
       m_SubPages[i] = tmpSubPages[i];

       //Print(cout);
       pChild1->Destroy();
       pChild2->Destroy();
       pChild3->Destroy();

       return bt_rootmerged;
}

template <typename keyType, typename ObjIDType>
typename CBTreePage<keyType, ObjIDType>::ObjectInfo &
CBTreePage<keyType, ObjIDType>::GetFirstObjectInfo()
{
        if( m_SubPages[0] )
                return m_SubPages[0]->GetFirstObjectInfo();
        return m_Keys[0];
}

template <typename keyType, typename ObjIDType>
void Print(tagObjectInfo<keyType, ObjIDType> &info, int level, void *pExtra)
{
       ostream &os = *(ostream *)pExtra;
       for(int i = 0; i < level ; i++)
               os << "\t";
       os << info.key << "->" << info.ObjID << "\n";
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::Print(ostream & os)
{
       lpfnForEach2 lpfn = &::Print<keyType, ObjIDType>;
       ForEach(lpfn, 0, &os);
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::Create()
{
       Reset();
       m_Keys.resize(m_MaxKeys+1);
       m_SubPages.resize(m_MaxKeys+2, NULL);
       m_KeyCount = 0;
       m_MinKeys  = 2 * m_MaxKeys/3;
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::Reset()
{
        // TODO: #35 change int by size_t
       for( int i = 0 ; i < m_KeyCount ; i++ )
               delete m_SubPages[i];
       clear();
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::clear()
{
       //m_Keys.clear();
       //m_SubPages.clear();
       m_KeyCount = 0;
}

template <typename keyType, typename ObjIDType>
// TODO: #36 change int by size_t
CBTreePage<keyType, ObjIDType> * CreateBTreeNode (int maxKeys, bool unique)
{
       return new CBTreePage<keyType, ObjIDType> (maxKeys, unique);
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::MovePage(BTPage *pChildPage, vector<ObjectInfo> &tmpKeys,vector<BTPage *> &tmpSubPages)
{
        // TODO: #37 change int by size_t
       int nKeys = pChildPage->GetNumberOfKeys();
       int i = 0;
       for(i = 0; i < nKeys; i++ )
       {
                tmpKeys    .push_back(pChildPage->m_Keys[i]);
                tmpSubPages.push_back(pChildPage->m_SubPages[i]);
       }
       tmpSubPages.push_back(pChildPage->m_SubPages[i]);
       pChildPage->clear();
}

#endif