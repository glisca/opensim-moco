function exampleSitToStand

import org.opensim.modeling.*;

keyboard

%% Predictive problem
moco = configureMocoTool();
problem = moco.updProblem();
model = getTorqueDrivenModel();
problem.setModelCopy(model);

problem.addCost(MocoControlCost('effort'));

solver = MocoCasADiSolver().safeDownCast(moco.updSolver());
solver.resetProblem(problem);
solver.createGuess('bounds');

predictSolution = moco.solve();
predictSolution.write('predictSolution.sto');
% moco.visualize(predictSolution);

%% Tracking problem
moco = configureMocoTool();
problem = moco.updProblem();
model = getTorqueDrivenModel();
problem.setModelCopy(model);

tracking = MocoStateTrackingCost();
tracking.setName('tracking');
tracking.setReferenceFile('predictSolution.sto');
tracking.setAllowUnusedReferences(true);
tracking.setWeight('/jointset/patellofemoral_r/knee_angle_r_beta/value', 0);
tracking.setWeight('/jointset/patellofemoral_r/knee_angle_r_beta/value', 0);
problem.addCost(tracking);

solver = MocoCasADiSolver().safeDownCast(moco.updSolver());
% solver.set_optim_constraint_tolerance(1e-6);
solver.resetProblem(problem);
solver.setGuess(predictSolution);

trackingSolution = moco.solve();
trackingSolution.write('trackingSolution.sto');
% moco.visualize(trackingSolution);


figure(1);
stateNames = predictSolution.getStateNames();
numStates = stateNames.size();
dim = ceil(sqrt(numStates));
for i = 0:numStates-1
    subplot(dim, dim, i+1);
    plot(predictSolution.getTimeMat(), ...
         predictSolution.getStateMat(stateNames.get(i)), '-r', ...
         'linewidth', 2);
    hold on
    plot(trackingSolution.getTimeMat(), ...
         trackingSolution.getStateMat(stateNames.get(i)), '--b', ...
         'linewidth', 1.5);
    hold off
    title(stateNames.get(i).toCharArray', 'Interpreter', 'none');
end

figure(2);
controlNames = predictSolution.getControlNames();
numControls = controlNames.size();
dim = ceil(sqrt(numControls));
for i = 0:numControls-1
    subplot(dim, dim, i+1);
    plot(predictSolution.getTimeMat(), ...
         predictSolution.getControlMat(controlNames.get(i)), '-r', ...
         'linewidth', 2);
    hold on
    plot(trackingSolution.getTimeMat(), ...
         trackingSolution.getControlMat(controlNames.get(i)), '--b', ...
         'linewidth', 1.5);
    hold off
    title(controlNames.get(i).toCharArray', 'Interpreter', 'none');
end



%% Muscle-driven inverse problem


%% Muscle-driven predictive problem

end

function [moco] = configureMocoTool()

import org.opensim.modeling.*;

moco = MocoTool();

% Configure the solver.
solver = moco.initCasADiSolver();
solver.set_num_mesh_points(50);
solver.set_dynamics_mode('implicit');
solver.set_optim_convergence_tolerance(1e-4);
solver.set_optim_constraint_tolerance(1e-4);
solver.set_optim_solver('ipopt');
solver.set_transcription_scheme('hermite-simpson');
solver.set_enforce_constraint_derivatives(true);
solver.set_optim_hessian_approximation('limited-memory');

% Set bounds on the problem.
problem = moco.updProblem();
problem.setTimeBounds(0, 1);
problem.setStateInfo('/jointset/hip_r/hip_flexion_r/value', ...
    MocoBounds(-2, 0.5), MocoInitialBounds(-2), MocoFinalBounds(0));
problem.setStateInfo('/jointset/knee_r/knee_angle_r/value', ...
    [-2, 0], -2, 0);
problem.setStateInfo('/jointset/ankle_r/ankle_angle_r/value', ...
    [-0.5, 0.7], -0.5, 0);

end

function [model] = getTorqueDrivenModel()

import org.opensim.modeling.*;

% Load the base model.
model = Model('sitToStand_3dof9musc.osim');

% Remove the muscles in the model.
model.updForceSet().clearAndDestroy();
model.initSystem();

% Add CoordinateActuators to the model degrees-of-freedom.
addCoordinateActuator(model, 'hip_flexion_r', 25);
addCoordinateActuator(model, 'knee_angle_r', 25);
addCoordinateActuator(model, 'ankle_angle_r', 25);

end

function addCoordinateActuator(model, coordName, optForce)

import org.opensim.modeling.*;

coordSet = model.updCoordinateSet();

actu = CoordinateActuator();
actu.setName(['tau_' coordName]);
actu.setCoordinate(coordSet.get(coordName));
actu.setOptimalForce(optForce);
actu.setMinControl(-1);
actu.setMaxControl(1);

model.addComponent(actu);

end

% function [model] = getMuscleDrivenModel()
% 
% import org.opensim.modeling.*;
% 
% % Load the base model.
% model = Model('sitToStand_3dof9musc.osim');
% 
% model.finalizeConnections();
% DeGrooteFregly2016Muscle().replaceMuscles(model);
% for m = 0:model.getMuscles()-1
%     musc = model.updMuscles().get(m);
%     musc.set_ignore_activation_dynamics(true);
%     musc.set_ignore_tendon_compliance(true);
% end
% 
% end