
/**
 *   The purpose of this test is to trigger error conditions
 *   of the state machine and verify that they behave as expected.
 *   
 */

#include "igstkStateMachine.h"

#include <iostream>




class Tester1
{
public:

  typedef igstk::StateMachine< Tester1 >   StateMachineType;

  typedef StateMachineType::TMemberFunctionPointer        ActionType;

  FriendClassMacro(StateMachineType);

  Tester1()
    {
    // Connect the state machine to 'this' class.
    m_StateMachine.SetOwnerClass( this );

    // Set the state descriptors
    m_StateMachine.AddState( "IdleState" );
    m_StateMachine.AddState( "OneQuarterCredit" );
    m_StateMachine.AddState( "TwoQuarterCredit" );
    m_StateMachine.AddState( "ThreeQuarterCredit" );

    // Set the input descriptors
    m_StateMachine.AddInput( "QuarterInserted");
    m_StateMachine.AddInput( "SelectDrink");
    m_StateMachine.AddInput( "CancelPurchase");

    const ActionType NoAction = 0;

    // Programming the machine
    m_StateMachine.AddTransition( "IdleState",          "QuarterInserted", "OneQuarterCredit",    NoAction );
    m_StateMachine.AddTransition( "OneQuarterCredit",   "QuarterInserted", "TwoQuarterCredit",    NoAction );
    m_StateMachine.AddTransition( "TwoQuarterCredit",   "QuarterInserted", "ThreeQuarterCredit",  NoAction );
    m_StateMachine.AddTransition( "ThreeQuarterCredit", "QuarterInserted", "IdleState",           & Tester1::CancelAndReturnChange );

    m_StateMachine.AddTransition(  "IdleState",          "SelectDrink",     "IdleState",          & Tester1::NoEnoughChangeMessage );
    m_StateMachine.AddTransition(  "OneQuarterCredit",   "SelectDrink",     "OneQuarterCredit",   & Tester1::NoEnoughChangeMessage );
    m_StateMachine.AddTransition(  "TwoQuarterCredit",   "SelectDrink",     "TwoQuarterCredit",   & Tester1::NoEnoughChangeMessage );
    m_StateMachine.AddTransition(  "ThreeQuarterCredit", "SelectDrink",     "IdleState",          & Tester1::DeliverDrink );

    m_StateMachine.AddTransition(  "IdleState",          "CancelPurchase",  "IdleState",          & Tester1::CancelAndReturnChange );
    m_StateMachine.AddTransition(  "OneQuarterCredit",   "CancelPurchase",  "IdleState",          & Tester1::CancelAndReturnChange );
    m_StateMachine.AddTransition(  "TwoQuarterCredit",   "CancelPurchase",  "IdleState",          & Tester1::CancelAndReturnChange );
    m_StateMachine.AddTransition(  "ThreeQuarterCredit", "CancelPurchase",  "IdleState",          & Tester1::CancelAndReturnChange );


    m_StateMachine.SelectInitialState( "IdleState");

    // Finish the programming and get ready to run
    m_StateMachine.SetReadyToRun();
    }

  void triggerError1()
  {
    m_StateMachine.SelectInitialState("ThisStateDoesntExists");
  }

protected:

  // Reaction methods that will be invoked during the 
  // state machine transitions. Note that this methods
  // are protected because they should only be called
  // from the state machine.
  void CancelAndReturnChange() 
    {
    std::cout << "Cancelling and returning Change" << std::endl;
    }


  void DeliverDrink()
    {
    std::cout << "Deliver Drink" << std::endl;
    }


  void NoEnoughChangeMessage()
    {
    std::cout << "You have not inserted enough change" << std::endl;
    }


private:

  StateMachineType   m_StateMachine;

};





class Tester2
{
public:

  typedef igstk::StateMachine< Tester2 >   StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer        ActionType;
  FriendClassMacro(StateMachineType);

  Tester2()
    {
    // On purpose NOT calling : m_StateMachine.SetOwnerClass( this );
    m_StateMachine.AddState( "IdleState" );
    m_StateMachine.SelectInitialState( "IdleState");
    m_StateMachine.SetReadyToRun();
    }

  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.ProcessInput( "QuarterInserted" );
    }
 
private:
  StateMachineType   m_StateMachine;
};




class Tester3
{
public:

  typedef igstk::StateMachine< Tester3 >   StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer        ActionType;
  FriendClassMacro(StateMachineType);

  Tester3()
    {
    m_StateMachine.SetOwnerClass( this );
    m_StateMachine.AddState( "IdleState" );
    m_StateMachine.SelectInitialState( "IdleState");
    m_StateMachine.AddInput( "QuarterInserted");
    const ActionType NoAction = 0;
    m_StateMachine.AddTransition( "IdleState", "QuarterInserted", "IdleState", NoAction );
    // On purpose NOT calling : m_StateMachine.SetReadyToRun();
    }

  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.ProcessInput( "QuarterInserted" );
    }
 
private:
  StateMachineType   m_StateMachine;
};



class Tester4
{
public:

  typedef igstk::StateMachine< Tester4 >   StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer        ActionType;
  FriendClassMacro(StateMachineType);

  Tester4()
    {
    m_StateMachine.SetOwnerClass( this );
    m_StateMachine.AddState( "IdleState" );
    m_StateMachine.SelectInitialState( "IdleState");
    m_StateMachine.AddInput( "QuarterInserted");
    const ActionType NoAction = 0;
    m_StateMachine.AddTransition( "IdleState", "QuarterInserted", "IdleState", NoAction );

    // On purpose invode SetReadyToRun() twice.
    m_StateMachine.SetReadyToRun();
    m_StateMachine.SetReadyToRun();
    }

  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.ProcessInput( "QuarterInserted" );
    }
 
private:
  StateMachineType   m_StateMachine;
};


class Tester5
{
public:

  typedef igstk::StateMachine< Tester5 >   StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer        ActionType;
  FriendClassMacro(StateMachineType);

  Tester5()
    {
    m_StateMachine.SetOwnerClass( this );
    m_StateMachine.AddState( "IdleState" );
    m_StateMachine.SelectInitialState( "IdleState");
    m_StateMachine.AddInput( "QuarterInserted");
    const ActionType NoAction = 0;
    // On purpose NOT adding any transition: for testing error condition.
    // m_StateMachine.AddTransition( "IdleState", "QuarterInserted", "IdleState", NoAction );
    m_StateMachine.SetReadyToRun();
    }

  void InvokeUndefinedTransition() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.ProcessInput( "QuarterInserted" );
    }
 
private:
  StateMachineType   m_StateMachine;
};




int igstkStateMachineErrorsTest(int argc, char * argv [])
{

  std::cout << "Construct the State Machine Tester" << std::endl;
  Tester1  tester1;

  std::cout << std::endl << std::endl;
  std::cout << "Trigger error conditions on purpose" << std::endl;

  // The following call are designed for testing the error conditions of the
  // state machine. 

  std::cout << "Invoking as Initial state a state that doesn't exist." << std::endl;
  tester1.triggerError1();

  std::cout << "Invoking SetReadyToRun() (in constructor) without parent class connected." << std::endl;
  Tester2 tester2;

  std::cout << "Invoking ProcessInput() without parent class connected." << std::endl;
  tester2.InsertChange();

  std::cout << "Invoking ProcessInput() without having called SetReadyToRun() ." << std::endl;
  Tester3 tester3;
  tester3.InsertChange();

  std::cout << "Invoking  SetReadyToRun() twice." << std::endl;
  Tester4 tester4;
  tester4.InsertChange();

  std::cout << "Invoking  ProcessInput() in a state without transitions defined." << std::endl;
  Tester5 tester5;
  tester5.InvokeUndefinedTransition();

  // The following call 
  return EXIT_SUCCESS;

}

