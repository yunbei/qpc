/*! @page struct Structure and Concepts

@tableofcontents

@section event_driven Event-Driven Programming
Virtually all embedded systems are **event-driven** by nature, which means that they continuously wait for the occurrence of some internal or external event such as a time tick, an arrival of some data, a button press, or a touch on a touch-screen. After recognizing the event, such systems *react* by performing the appropriate computation that may include manipulating the hardware or generating "soft" events that trigger other internal software components. Once the event-handling is complete, the software goes back to waiting for the next event.

However, most embedded systems are traditionally programmed in a **sequential** manner, where a program waits for *specific* events in various places in its execution path by either busy-polling or blocking on semaphore or other such mechanism of a traditional Real-Time Operating System (RTOS). Example of the basic sequential code is the traditional "Blinky" implementation:

@code{.c}
while (1) { /* RTOS task or a "superloop" */
    BSP_ledOn();     /* turn the LED on  */
    RTOS_delay(400); /* wait for 400 ms  */
    BSP_ledOff();    /* turn the LED off */
    RTOS_delay(600); /* wait for 600 ms  */
}
@endcode

Although this approach is functional in many situations, it does not work very well when there are multiple possible events whose arrival times and order you cannot predict and where it is important to handle the events in a timely manner. The fundamental problem is that while a sequential program is waiting for one kind of event (e.g., elapsing of a time interval) it is not doing anything else and is **not responsive** to other events (e.g., a button press).

For this and other reasons experienced developers turn to the long-known design strategy called **event-driven programming**, which requires a distinctly different way of thinking than conventional sequential programs. All event-driven programs are naturally divided into the application, which actually handles the events, and the supervisory event-driven infrastructure (**framework**), which waits for events and dispatches them to the application. The control resides in the event-driven framework, so from the application standpoint, the control is *inverted* compared to a traditional sequential program.

The **QP/C framework** brings together two most effective techniques of structuring event-driven embedded systems: <a href="http://www.state-machine.com/doc/concepts.html#Active" target="_blank" class="extern">active objects</a> and <a href="http://www.state-machine.com/doc/concepts.html#HSM" target="_blank" class="extern">hierarchical state machines</a> (UML statecharts). The following sections describe the main components and structure of the framework.


------------------------------------------------------------------------------
@section comp Components of QP/C
<p>As shown in the diagram below, the QP/C framework has a layered structure. The Target hardware sits at the bottom. The Support Package (BSP) above it provides access to the board-specific features, such as the peripherals. The real-time kernel (QV, QK, QXK, or a conventional 3rd-party RTOS) provides the foundation for multitasking, such as task scheduling, context-switching, and inter-task communication. Based on these services, the event-driven framework (QF) supplies the event-driven infrastructure for executing <a href="http://www.state-machine.com/doc/concepts.html#Active" target="_blank" class="extern">active objects</a> and ensuring thread-safe event-driven exchanges among them. Finally, the event-processor (QEP) implements the hierarchical state machine semantics (based on UML statecharts). The top layer is the application-level code consisting of loosely-coupled active objects.
</p>

@image html qp_components.jpg "Components of the QP Framework"
@n

<div class="separate"></div>
@subsection comp_qep QEP Hierarchical Event Processor
QEP is a universal, UML-compliant event processor that provides implementation of <a href="http://www.state-machine.com/doc/concepts.html#HSM" target="_blank" class="extern">hierarchical state machines</a> (UML statecharts) in highly readable ANSI-C. The hallmark of QEP implementation strategy is **traceability**, which means that every state machine element is mapped to code precisely, unambiguously, and exactly once. QEP fully supports hierarchical state nesting, which is the fundamental mechanism for reusing behavior across many states instead of repeating the same actions and transitions over and over again. (<span class="highlight">See @ref qep for detailed documentation</span>).

<div class="separate"></div>
@subsection comp_qf QF Active-Object Framework
QF is a lightweight, event-driven, <a href="http://www.state-machine.com/doc/concepts.html#Framework" target="_blank" class="extern">active object framework</a> specifically designed for real-time embedded (RTE) systems. The main job of the framework is  to guarantee **thread-safe**, run-to-completion event processing within each <a href="http://www.state-machine.com/doc/concepts.html#Active" target="_blank" class="extern">active object</a>. This includes direct event posting as well as publish-subscribe event delivery, event queuing, and time events (time-delayed requests for posing events). (<span class="highlight">See @ref qf  for detailed documentation</span>).

<div class="separate"></div>
@subsection comp_qv  QV Cooperative Kernel
QV is a simple cooperative kernel (previously called "Vanilla" kernel). This kernel always processes one event at a time to completion, and performs priority-based scheduling of active objects after processing of each event. The QV kernel is "implicitly-cooperative", because the active object do not need to yield the CPU explicitly. Instead, they simply return to the QV scheduler after completion of event processing. Due to naturally short duration of event processing in state machines, the simple QV kernel is often adequate for many real-time systems. (<span class="highlight">See @ref qv for detailed documentation</span>). This is the fastest, smallest, and easiest-to-understand way of executing active objects.

<div class="separate"></div>
@subsection comp_qk QK Preemptive Non-Blocking Kernel
QK is an ultra-fast **preemptive**, priority-based, single-stack, real-time kernel designed specifically for executing active objects. The QK kernel always executes the highest-priority active object that has event(s) queued up, but it processes events as one-shot function calls (instead of endless loops, as traditional RTOS kernels). Still, the QK kernel allows these one-shot event-processing functions to preempt each other, if the priority of the new event is higher than the currently-processed event (very much like prioritized interrupt controllers allow interrupts to preempt each other). This means that QK can use _single stack_ for keeping the context all active objects (in the same way as prioritized interrupt controllers use a single stack to nest all interrupts). QK meets all the requirement of the <a class="extern" target="_blank" href="http://en.wikipedia.org/wiki/Rate-monotonic_scheduling"><strong>Rate Monotonic Scheduling</strong></a> (a.k.a. Rate Monotonic Analysis &mdash; RMA) and can be used in hard real-time systems. (<span class="highlight">See @ref qk for detailed documentation</span>).

<div class="separate"></div>
@subsection comp_qxk QXK Preemptive Blocking Kernel
QXK is a simple preemptive, priority-based, **blocking**, real-time kernel designed specifically for mixing active objects with traditional blocking code, such as commercial middleware (TCP/IP stacks, UDP stacks, embedded file systems, etc.) or legacy code. QXK works like most conventional **RTOS** kernels, and like most of such kernels requires every thread (active object and every "naked" thread) to provide a separate private stack. The kernel provides a usual assortment of blocking facilities, such as timed-delays, semaphores, mutexes, and blocking message queues. QXK meets all the requirement of the <a class="extern" target="_blank" href="http://en.wikipedia.org/wiki/Rate-monotonic_scheduling"><strong>Rate Monotonic Scheduling</strong></a> (a.k.a. Rate Monotonic Analysis &mdash; RMA) and can be used in hard real-time systems. (<span class="highlight">See @ref qxk for detailed documentation</span>).

@note
QXK has most features you might expect of a traditional blocking RTOS kernel and is recommended as the preferred RTOS kernel for QP/C applications that need to mix active objects with traditional blocking code. Due to the tight and optimal integration between QXK and the rest of QP/C, QXK offers better performance and smaller memory footprint than any QP port to a @ref ports_rtos "3rd-party RTOS".


<div class="separate"></div>
@subsection comp_qs QS Software Tracing System
QS is software tracing system that enables developers to monitor live event-driven QP applications with minimal target system resources and without stopping or significantly slowing down the code. QS is an ideal tool for testing, troubleshooting, and optimizing QP applications. QS can even be used to support acceptance testing in product manufacturing. (<span class="highlight">See @ref qs for detailed documentation</span>).


------------------------------------------------------------------------------
@section oop Object-Orientation

Even though QP/C is implemented in standard ANSI-C, it extensively uses object-oriented design principles such as **encapsulation** (classes), single **inheritance**, and, starting with QP5, **polymorphism** (late binding). At the C language level, these proven ways of software design become **design-patterns** and coding idioms.

@htmlonly
<div class="image">
<a target="_blank" href="http://www.state-machine.com/doc/AN_OOP_in_C.pdf"><img border="0" src="img/AN_OOP_in_C.jpg" title="Download PDF"></a>
<div class="caption">
Application Note: Object-Oriented Programming in C
</div>
</div>
@endhtmlonly

The Quantum Leaps Application Note <a class="extern" target="_blank" href="http://www.state-machine.com/doc/AN_OOP_in_C.pdf"><strong>Object-Oriented Programming in C</strong></a> describes how the OOP design patterns are implemented in QP/C and how you should code them in your own applications.
<div class="clear"></div>


------------------------------------------------------------------------------
@section classes Classes in QP/C
The figure below shows the main classes comprising the QP/C framework and their relation to the application-level code, such as the @ref game example application (shown at the bottom 1/3 of the diagram).

@image html qp_classes.gif "Main Classes in the QP Framework"

<ul class="tag">
  <li><span class="tag">0</span> The ::QEvt class represents events without parameters and serves as the base class for derivation of time events and any events with parameters. For example, application-level events `ObjectPosEvt` and `ObjectImageEvt` inherit ::QEvt and add to it some parameters (see [8]).
  </li>

  <li><span class="tag">1</span> The abstract ::QMsm class represents the most fundamental State Machine in QP/C. This class implements the fastest and the most efficient strategy for coding hierarchical state machines, but this strategy is not human-maintainable and requires the use of the <a class="extern" target="_blank" href="http://www.state-machine.com/qm">QM modeling tool</a>. The class is abstract, meaning that it is not designed to be instantiated directly, but rather only for inheritance. The @ref game application provides an example of application-level classes deriving directly from ::QMsm (see [7]).
  </li>

  <li><span class="tag">2</span> The abstract ::QHsm class derives from ::QMsm and implements the state machine coding strategy suitable for manual coding and maintaining the code. The ::QHsm strategy is also supported by the <a class="extern" target="_blank" href="http://www.state-machine.com/qm">QM modeling tool</a>, but is not as fast or efficient as the ::QMsm strategy.
  </li>

  <li><span class="tag">3</span> The abstract ::QMActive class represents an active object that uses the ::QMsm style state machine implementation strategy. This strategy requires the use of the QM modeling tool to generate state machine code automatically, but the code is faster than in the ::QHsm style implementation strategy and needs less run-time support (smaller event-processor).
  </li>

  <li><span class="tag">4</span> The abstract ::QActive class represents an active object that uses the ::QHsm style implementation strategy for state machines. This strategy is tailored to manual coding, but it is also supported by the QM modeling tool. The resulting code is slower than in the ::QMsm-style implementation strategy.
  </li>

  <li><span class="tag">5</span> The ::QTimeEvt class represents time events in QP. **Time events** are special QP events equipped with the notion of time passage. The basic usage model of the time events is as follows. An active object allocates one or more ::QTimeEvt objects (provides the storage for them). When the active object needs to arrange for a timeout, it arms one of its time events to fire either just once (one-shot) or periodically. Each time event times out independently from the others, so a QP application can make multiple parallel timeout requests (from the same or different active objects). When QP detects that the appropriate moment has arrived, it inserts the time event directly into the recipient's event queue. The recipient then processes the time event just like any other event.
  </li>

  <li><span class="tag">6</span> Active Objects in the application derive either from the ::QMActive or ::QActive base class.
  </li>

  <li><span class="tag">7</span> Applications can also use classes derived directly from the ::QMsm or ::QHsm base classes to represent "raw" state machines that are not active objects, because they don't have event queue and execution thread. Such "raw" state machines are typically used as "Orthogonal Components".
  </li>

  <li><span class="tag">8</span> Application-level events with parameters derive from the ::QEvt class.
  </li>

</ul>


------------------------------------------------------------------------------
@section sm State Machines

The behavior of each active object in QP/C is specified by means of a **hierarchical state machine** (UML statechart), which is the most effective and elegant technique of decomposing event-driven behavior. The most important innovation of UML state machines over classical finite state machines (FSMs) is the hierarchical state nesting. The value of state nesting lies in avoiding repetitions, which are inevitable in the traditional "flat" FSM formalism and are the main reason for the "state-transition explosion" in FSMs. The semantics of state nesting allow substates to define only the differences of behavior from the superstates, thus promoting sharing and reusing behavior.

@htmlonly
<div class="image">
<a target="_blank" href="http://www.state-machine.com/doc/AN_Crash_Course_in_UML_State_Machines.pdf"><img border="0" src="img/AN_Crash_Course_in_UML_State_Machines.jpg" title="Download PDF"></a>
<div class="caption">
Application Note: A Crash Course in UML State Machines
</div>
</div>
@endhtmlonly

The Quantum Leaps Application Note <a class="extern" target="_blank" href="http://www.state-machine.com/doc/AN_Crash_Course_in_UML_State_Machines.pdf"><strong>A Crash Course in UML State Machines</strong></a> introduces the main state machine concepts backed up by examples.
<div class="clear"></div>

@note
The hallmark of the QP/C implementation of UML state machines is **traceability**, which is direct, precise, and unambiguous mapping of every state machine element to human-readable, portable, MISRA-compliant C code. Preserving the traceability from requirements through design to code is essential for mission-critical systems, such as medical devices or avionic systems.


------------------------------------------------------------------------------
@section coding Coding Standard
The QP/C framework has been developed in strict adherence to the documented <a class="extern" target="_blank" href="http://www.state-machine.com/doc/AN_QL_Coding_Standard.pdf"><strong>Quantum Leaps Coding Standard</strong></a>.

@htmlonly
<div class="image">
<a target="_blank" href="http://www.state-machine.com/doc/AN_QL_Coding_Standard.pdf"><img border="0" src="img/AN_Coding_Standard.jpg " title="Download PDF"></a>
<div class="caption">
Application Note: Quantum Leaps C/C++ Coding Standard
</div>
</div>
@endhtmlonly


------------------------------------------------------------------------------
@section misra MISRA Compliance

<p>The QP/C framework complies with most of the Motor Industry Software Reliability Association (MISRA) MISRA-C:2004 rules.
</p>

@htmlonly
<div class="image">
<a target="_blank" href="http://www.state-machine.com/doc/AN_QP-C_MISRA.pdf"><img border="0" src="img/AN_MISRA-QPC.jpg" title="Download PDF"></a>
<div class="caption">
Application Note: QP/C MISRA-C:2004 Compliance Matrix
</div>
</div>
@endhtmlonly

All deviations are carefully limited into very specific contexts and are documented with the Application Note: <a class="extern" target="_blank" href="http://www.state-machine.com/doc/AN_QP-C_MISRA.pdf"><strong>QP/C MISRA-C:2004 Compliance Matrix</strong></a>.

@note
MISRA and MISRA C are registered trademarks of MIRA Ltd, held on behalf of the MISRA Consortium.

The MISRA guidelines place great emphasis on the use of static code analysts tools to check compliance with the MISRA-C language subset. To this end, QP/C comes with an extensive support for automatic rule checking with @ref lint "PC-Lint". The QP frameworks go even beyond MISRA, by complying with the <strong>strict type checking</strong> of PC-Lint.


------------------------------------------------------------------------------
@section pc-lint PC-Lint Support

The QP/C framework comes with extensive support for automatic rule checking by means of <a class="extern" target="_blank" href="http://www.gimpel.com/">PC-Lint</a>, which is designed not just for proving compliance of the Q/CP framework code, but more importantly, to aid in checking compliance of the application-level code. Any organization engaged in designing safety-related embedded software could benefit from the unprecedented quality infrastructure built around the QP/C framework.

@sa @ref lint "Lint Port"


@next{exa}
*/

