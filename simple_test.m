% Create and test axon section
axon = nrn_section_new('axon4');
nrn_section_push(axon);

% Test section length
length_of_axon = nrn_section_length_get(axon);
disp(['Axon4 Length: ', num2str(length_of_axon)]);
nrn_section_length_set(axon, 20.2);
length_of_axon = nrn_section_length_get(axon);
disp(['Length of Inputted Axon: ', num2str(length_of_axon)]);

% Test nseg
nseg_axon = nrn_nseg_get(axon);
disp(['Nseg of axon: ', num2str(nseg_axon)]);
nrn_nseg_set(axon, 17.5);
nseg_axon = nrn_nseg_get(axon);
disp(['Nseg of axon after setting: ', num2str(nseg_axon)]);

% Test RA
ra_axon = nrn_section_Ra_get(axon);
disp(['RA axon: ', num2str(ra_axon)]);
nrn_section_Ra_set(axon, 24.5);
ra_axon = nrn_section_Ra_get(axon);
disp(['RA axon after setting: ', num2str(ra_axon)]);
% Set diameter
nrn_segment_diam_set(axon, 0.5, 1.5);

% Create and connect sections
parent_sec = nrn_section_new('parent');
child_sec = nrn_section_new('child');
disp('Connecting sections...');
nrn_section_connect(child_sec, 0, parent_sec, 1);
nrn_section_connect(axon, 0, parent_sec, 1);

% Print section info once
disp('Section Information:');
nrn_hoc_call('forall psection()');

% Run clamp test
disp('Running test_clamp...');
test_iclamp();

% Helper Functions
function sec = nrn_section_new(name)
  sec = neuron_api('nrn_section_new', name);
end

function nrn_double_push(val)
  % Push a double value onto the NEURON stack
  neuron_api('nrn_double_push', val);
end

function nrn_hoc_call(command)
  neuron_api('nrn_hoc_call', command);
end

function nrn_section_push(sec)
  neuron_api('nrn_section_push', sec);
end

function nrn_nseg_set(sec, nseg)
  neuron_api('nrn_nseg_set', sec, nseg);
end

function nrn_function_call(func_name, nargs)
    % Helper function to call NEURON functions using HOC
    % func_name: string name of function
    % nargs: number of arguments
    
    % Create the function call string
    call_str = sprintf('%s()', func_name);
    neuron_api('nrn_function_call', call_str);
end


function nsegment = nrn_nseg_get(sec)
  nsegment = neuron_api('nrn_nseg_get', sec); % Retrieve and return nseg
end

function nrn_section_Ra_set(sec, ra)
  neuron_api('nrn_section_Ra_set', sec, ra);
end

function ra_length = nrn_section_Ra_get(sec)
  ra_length = neuron_api('nrn_section_Ra_get', sec); % Retrieve and return RA
end

function nrn_section_length_set(sec, L)
  neuron_api('nrn_section_length_set', sec, L);
end

function length = nrn_section_length_get(sec)
  length = neuron_api('nrn_section_length_get', sec); % Retrieve and return length
end

function name = nrn_secname(sec)
  name = neuron_api('nrn_secname', sec); % Get the name of the section
end

function nrn_segment_diam_set(sec, x, diam)
  neuron_api('nrn_segment_diam_set', sec, x, diam);
end

function diam = nrn_segment_diam_get(sec, x)
  diam = neuron_api('nrn_segment_diam_get', sec, x);
end

function nrn_rangevar_set(sec, value)
  neuron_api('nrn_rangevar_set', sec, value);
end

function value = nrn_rangevar_get(sec)
  value = neuron_api('nrn_rangevar_get', sec);
end

function nrn_section_pop()
  neuron_api('nrn_section_pop'); % Pop the current section
end

function nrn_section_connect(child_sec, child_x, parent_sec, parent_x)
  % Connect the child section to the parent section
  neuron_api('nrn_section_connect', child_sec, child_x, parent_sec, parent_x);
end
function nrn_str_push(str)
    neuron_api('nrn_str_push', str);
end

function nrn_int_push(val)
    neuron_api('nrn_int_push', val);
end

function nrn_symbol_push(sym)
    neuron_api('nrn_symbol_push', sym);
end

function sym = nrn_symbol(name)
    sym = neuron_api('nrn_symbol', name);
end

function result = nrn_double_pop()
    result = neuron_api('nrn_double_pop');
end

function obj = nrn_object_pop()
    obj = neuron_api('nrn_object_pop');
end

function nrn_mechanism_insert(sec, sym)
    neuron_api('nrn_mechanism_insert', sec, sym);
end

function test_iclamp()
    % Load mechanisms and initialize
    neuron_api('nrn_hoc_call', 'load_file("stdrun.hoc")');
    
    % Create and setup soma
    soma = neuron_api('nrn_section_new', 'soma');
    neuron_api('nrn_nseg_set', soma, 3);
    neuron_api('nrn_section_push', soma);
    neuron_api('nrn_hoc_call', 'pt3dadd(0, 0, 0, 10)');
    neuron_api('nrn_hoc_call', 'pt3dadd(10, 0, 0, 10)');
    neuron_api('nrn_hoc_call', 'insert hh');
    
    % Create IClamp
    neuron_api('nrn_hoc_call', 'objectvar ic');
    neuron_api('nrn_hoc_call', 'ic = new IClamp(0.5)');
    neuron_api('nrn_hoc_call', 'ic.amp = 0.3');
    neuron_api('nrn_hoc_call', 'ic.del = 1');
    neuron_api('nrn_hoc_call', 'ic.dur = 0.1');
    
    % Create vector and record
    neuron_api('nrn_hoc_call', 'objectvar vec');
    neuron_api('nrn_hoc_call', 'vec = new Vector()');
    neuron_api('nrn_hoc_call', 'vec.record(&v(0.5))');
    
    % Run simulation
    neuron_api('nrn_hoc_call', 'finitialize(-65)');
    neuron_api('nrn_double_push', 10.5);
    neuron_api('nrn_hoc_call', 'continuerun(10.5)');
    
    % Print voltage values only
    fprintf('Voltage readings:\n');
    neuron_api('nrn_hoc_call', 'printf("t=0ms: %g mV\n", vec.x[0])');
    neuron_api('nrn_hoc_call', 'printf("t=5ms: %g mV\n", vec.x[1])');
    neuron_api('nrn_hoc_call', 'printf("t=10ms: %g mV\n", vec.x[2])');
end
